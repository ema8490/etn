package io

import (
	"bytes"
	"ethos/elTypes"
	"ethos/ethos"
	"ethos/kernelTypes"
	"ethos/parser"
	"ethos/syscall"
	"ethos/typeDefs"
	"ethos/types"
	"ethos/util"
	"reflect"
	"strings"
)

var typeGraph []kernelTypes.TypeNode

func typesPath() string {
	return "/types/all"
}

func RegisterTypes() {
	for ty, name := range t.ETN_NAMES {
		t.Register(ty, GetHash(name))
	}
}

func accessOrCreate(path string, hash []byte) syscall.Fd {
	dirFd, status := ethos.OpenDirectoryPath(path)
	if status != syscall.StatusOk {
		//TODO: fix this using CreateDirectoryPath
		parts := strings.Split(path, "/")
		pathLastDir := strings.Join(parts[:len(parts)-1], "/")
		dirName := parts[len(parts)-1]
		lastDirFd, openStatus := ethos.OpenDirectoryPath(pathLastDir)
		if openStatus != syscall.StatusOk {
			util.Printf("Error opening directory %s\n", pathLastDir)
			return 0
		}
		status = ethos.CreateDirectory(lastDirFd, dirName, "", hash[:])
		if status != syscall.StatusOk {
			util.Printf("Error creating directory %s\n", path)
			return 0
		}
		newDirFd, newStatus := ethos.OpenDirectoryPath(path)
		if newStatus != syscall.StatusOk {
			util.Printf("Error opening directory %s\n", path)
			return 0
		}
		return newDirFd
	}

	if bytes.Compare(GetDirHash(path), hash) != 0 {
		return 0
	}

	return dirFd
}

func getGraph() []kernelTypes.TypeNode {
	if typeGraph == nil {
		Init()
		typeGraph = readTypeGraph()
	}
	return typeGraph
}

func GetTypeName(path string) string {
	if !IsDirectory(path) {
		parts := strings.Split(path, "/")
		path = strings.Join(parts[:len(parts)-1], "/")
	}

	return GetDirTypeName(path)
}

func GetDirTypeName(path string) string {
	return GetName(GetDirHash(path))
}

func GetDirHash(path string) []byte {
	fileInfoEtn, status := ethos.GetFileInformationPath(path)
	if status != syscall.StatusOk {
		// util.Printf("Error calling FileInformation %s\n", path)
		return []byte{}
	}
	reader := bytes.NewBuffer(fileInfoEtn)
	d := kernelTypes.NewDecoder(reader)
	d.ReadAll()

	fileInfo, err := d.FileInformation()
	if err != nil {
		ReadTypedDir(path)
		util.Printf("Error decoding FileInformation\n")
		return []byte{}
	}
	return fileInfo.TypeHash[:]
}

func GetHash(typeName string) []byte {
	tg := getGraph()
	hash := []byte{}
	found := false
	for _, ty := range tg {
		if ty.Name == typeName {
			if found {
				util.Fatalf("Multiple hash found for type %s\n", typeName)
				break
			}
			hash = ty.Hash
			found = true
		}
	}

	return hash
}

func GetName(hash []byte) string {
	tg := getGraph()
	for _, ty := range tg {
		if bytes.Compare(ty.Hash, hash) == 0 {
			return ty.Name
		}
	}
	util.Printf("Cannot find type with hash %v\n", hash)
	return ""
}

func readTypeGraph() []kernelTypes.TypeNode {
	dirFd, status := ethos.OpenDirectoryPath(typesPath())
	if status != syscall.StatusOk {
		util.Printf("Error opening %s\n", typesPath())
	}

	typeGraph := []kernelTypes.TypeNode{}

	lastName := ""
	for bytes, status := ethos.GetNextName(dirFd, lastName); status == syscall.StatusOk; bytes, status = ethos.GetNextName(dirFd, lastName) {
		if status == syscall.StatusNotFound {
			break
		}

		lastName = string(bytes)

		if lastName == "." || lastName == ".." || lastName == "" {
			continue
		}

		typeFile := ethos.NewVarReader(dirFd, lastName)
		d := kernelTypes.NewDecoder(typeFile)
		d.ReadAll()
		typ, err := d.TypeNode()
		if err != nil {
			util.Printf("Error on TypeNode()\n")
			return typeGraph
		}

		typeGraph = append(typeGraph, *typ)
	}

	return typeGraph
}

func GetTypeFields(name string) map[string]string {
	return GetTypeFieldsFromHash(GetHash(name))
}

func GetTypeFieldsFromHash(typeHash []byte) (dict map[string]string) {
	dict = make(map[string]string)
	index := 0
	tg := getGraph()

	typeNode := getTypeFromHash(typeHash, tg)
	if typeNode == nil {
		return nil
	}

	elems := getElemsPtrFromHashes(typeNode.Elems, tg)

	for _, f := range elems {
		//TODO : Assuming the types are always structs and this join doesn't work for nested structs
		if f.Name == "struct" {
			for _, k := range getElemsPtrFromHashes(f.Elems, tg) {
				dict[f.Fields[index]] = k.Name
				index++
			}

		}

	}

	return
}

func IsPrimitive(typeName string) bool {
	return IsPrimitiveFromHash(GetHash(typeName))
}

func IsPrimitiveFromHash(typeHash []byte) bool {
	tg := getGraph()

	typeNode := getTypeFromHash(typeHash, tg)
	if typeNode == nil {
		return false
	}

	if typeNode.Base == types.VANY {
		return true
	}

	return uint8(0) <= typeNode.Base && typeNode.Base < uint8(len(types.Builtins))
}

func GetTypeBaseName(typeName string) string {
	return GetTypeBaseNameFromHash(GetHash(typeName))
}

func GetTypeBaseNameFromHash(typeHash []byte) string {
	tg := getGraph()

	typeNode := getTypeFromHash(typeHash, tg)
	if typeNode == nil {
		return ""
	}

	return typeNode.Name
}

func GetTypeBase(baseName string) int {
	for k, v := range types.Builtins {
		if v.Name == baseName {
			return k
		}
	}
	return -1
}

func getElemsPtrFromHashes(h []kernelTypes.TGHashValue, tg []kernelTypes.TypeNode) (elems []*kernelTypes.TypeNode) {
	for _, e := range h {
		elems = append(elems, getTypeFromHash(e, tg))
	}
	return elems
}

func getTypeFromHash(h kernelTypes.TGHashValue, tg []kernelTypes.TypeNode) *kernelTypes.TypeNode {
	for _, t := range tg {
		if bytes.Equal([]byte(h[:]), t.Hash[:]) {
			return &t
		}
	}
	return nil
}

func ReadObject(path string) interface{} {
	path = MakePath(path, "", false)
	_, status := ethos.GetFileInformationPath(path)
	if status != syscall.StatusOk {
		return nil
	}

	parts := strings.Split(path, "/")
	dirPath := strings.Join(parts[:len(parts)-1], "/")
	typeName := GetDirTypeName(dirPath)
	if Mapping[typeName] == nil {
		util.Fatalf("unknonwn type %s\n", typeName)
	}

	dirFd, name, status := openPathLastDirectory(path)
	if status != syscall.StatusOk {
		util.Fatalf("Error opening %s\n", path)
	}

	reader := ethos.NewVarReader(dirFd, name)
	d := Mapping[typeName].NewDecoder.Call([]reflect.Value{reflect.ValueOf(reader)})[0]

	r := d.MethodByName(typeName).Call([]reflect.Value{})

	err := r[1]
	if !err.IsNil() {
		util.Printf("%v\n", err)
		return nil
	}

	return r[0]
}

func SimpleTypeName(obj reflect.Value) string {
	parts := strings.Split(obj.Type().String(), ".")
	return parts[len(parts)-1]
}

func WriteObject(o interface{}, path string) {
	obj, ok := o.(reflect.Value)
	if !ok {
		util.Printf("invalid obj type\n")
		return
	}

	typeName := SimpleTypeName(obj)
	if Mapping[typeName] == nil {
		util.Printf("unknonwn type %s\n", typeName)
		return
	}

	path, ok = MakePathExists(path, "", false)
	if !ok {
		path, ok = MakeNewPathExists(path, "", false)
		if !ok {
			util.Printf("Error opening %s\n", path)
			return
		}
	}

	dirFd, name, status := openPathLastDirectory(path)
	if status != syscall.StatusOk {
		util.Printf("Error opening %s\n", path)
		return
	}

	writer := ethos.NewVarWriter(dirFd, name)
	e := Mapping[typeName].NewEncoder.Call([]reflect.Value{reflect.ValueOf(writer)})[0]

	if obj.CanAddr() {
		e.MethodByName(typeName).Call([]reflect.Value{obj.Addr()})
	} else {
		e.MethodByName(typeName).Call([]reflect.Value{obj})
	}
	e.MethodByName("Flush").Call([]reflect.Value{})

	// TODO: signal error / panic
	// _, status = ethos.GetFileInformationPath(path)
	// if status != syscall.StatusOk {
	// }
}

func ReadPrimitive(path string) (interface{}, int) {
	path = MakePath(path, "", false)
	_, status := ethos.GetFileInformationPath(path)
	if status != syscall.StatusOk {
		return nil, t.UNDEFINED
	}

	dirFd, name, status := openPathLastDirectory(path)
	if status != syscall.StatusOk {
		util.Fatalf("Error opening %s\n", path)
	}

	baseName := GetTypeBaseName(GetTypeName(path))
	base := GetTypeBase(baseName)
	at := t.PrimitiveTypeFromBase(base)

	reader := ethos.NewVarReader(dirFd, name)
	decoder := kernelTypes.NewDecoder(reader)
	return decodePrimitive(at, decoder)
}

func CastPrimitive(at int, value reflect.Value) interface{} {

	switch at {
	case t.INT:
		return value.Int()
	case t.BOOL:
		return value.Bool()
	case t.FLOAT:
		return value.Float()
	case t.STRING:
		return value.String()
	default:
		util.Printf("cannot cast %v to primitive\n", value)
	}

	return nil
}

func decodePrimitive(at int, decoder *kernelTypes.Decoder) (interface{}, int) {
	var av, err interface{}

	switch at {
	case t.INT:
		v, e := decoder.Int64()
		av = *v
		err = e
	case t.BOOL:
		v, e := decoder.Bool()
		av = *v
		err = e
	case t.FLOAT:
		v, e := decoder.Float64()
		av = *v
		err = e
	case t.STRING:
		v, e := decoder.String()
		av = *v
		err = e
	}

	if err != nil {
		util.Printf("%v\n", err)
	}

	return av, at
}

func WritePrimitive(v interface{}, path string) {
	path = MakePath(path, "", false)
	dirFd, name, status := openPathLastDirectory(path)
	if status != syscall.StatusOk {
		util.Fatalf("Error opening %s\n", path)
	}

	writer := ethos.NewVarWriter(dirFd, name)
	encoder := kernelTypes.NewEncoder(writer)
	encodePrimitive(v, encoder)
}

func encodePrimitive(v interface{}, encoder *kernelTypes.Encoder) {
	switch av := v.(type) {
	case string:
		encoder.String(av)
	case int:
		encoder.Int64(int64(av))
	case int64:
		encoder.Int64(av)
	case bool:
		encoder.Bool(av)
	case float32:
		encoder.Float64(float64(av))
	case float64:
		encoder.Float64(av)
	}
	encoder.Flush()
}

// TODO: old stuff below

func writeComposite(value interface{}, kind int, encoder *typeDefs.Encoder) {

	switch kind {
	case t.TUPLE:
		tuple, _ := value.(*t.TupleRep)
		ev := typeDefs.ElTuple{Content: []typeDefs.Any{}, Type: typeDefs.ElInt(tuple.Type)}
		for _, v := range tuple.Content {
			tv, _ := v.(*parser.ParseTree)
			var encodedValue interface{}
			if !t.IsComposite(tuple.Type) {
				encodedValue = encodePrimitive_(tv.ActualValue, tuple.Type)
			} else {
				encodedValue = encodeComposite(tv.ActualValue, tuple.Type)
			}
			ev.Content = append(ev.Content, typeDefs.Any(encodedValue))
		}
		encoder.ElTuple(&ev)
		break
	}
	encoder.Flush()
}

func encodeComposite(value interface{}, kind int) interface{} {
	switch kind {
	case t.TUPLE:
		tuple, _ := value.(*t.TupleRep)
		ev := typeDefs.ElTuple{Content: []typeDefs.Any{}, Type: typeDefs.ElInt(tuple.Type)}
		for _, v := range tuple.Content {
			tv, _ := v.(*parser.ParseTree)
			var encodedValue interface{}
			if !t.IsComposite(tuple.Type) {
				encodedValue = encodePrimitive_(tv.ActualValue, tuple.Type)
			} else {
				encodedValue = encodeComposite(tv.ActualValue, tuple.Type)
			}
			ev.Content = append(ev.Content, typeDefs.Any(encodedValue))
		}
		return ev
	}
	util.Fatalf("Unknown composite type: %s\n", t.TYPES[kind])
	return nil
}

func encodePrimitive_(value interface{}, kind int) interface{} {
	switch kind {
	case t.INT:
		v, _ := value.(int)
		return typeDefs.ElInt(v)
	case t.FLOAT:
		v, _ := value.(float64)
		return typeDefs.ElFloat(v)
	case t.STRING:
		v, _ := value.(string)
		return typeDefs.ElString(v)
	case t.BOOL:
		v, _ := value.(bool)
		return typeDefs.ElBool(v)
	}
	util.Fatalf("Unknown primitive type: %s\n", t.TYPES[kind])
	return nil
}

func readComposite(kind int, decoder *typeDefs.Decoder) (interface{}, interface{}) {
	// util.Printf("readComposite\n")
	switch kind {
	case t.TUPLE:
		et, err := decoder.ElTuple()
		if err != nil {
			return nil, err
		}
		for i := 0; i < len(et.Content); i++ {
			if !t.IsComposite(int(et.Type)) {
				et.Content[i] = decodePrimitive_(int(et.Type), et.Content[i])
			} else {
				et.Content[i] = decodeComposite(int(et.Type), et.Content[i])
			}
		}
		return et, nil
	}
	return nil, nil
}

func decodePrimitive_(kind int, value interface{}) interface{} {
	ret := new(parser.ParseTree)
	ret.ActualType = kind
	switch kind {
	case t.INT:
		ev, _ := value.(typeDefs.ElInt)
		ret.ActualValue = int(ev)
		return ret
	case t.FLOAT:
		ev, _ := value.(typeDefs.ElFloat)
		ret.ActualValue = float64(ev)
		return ret
	case t.STRING:
		ev, _ := value.(typeDefs.ElString)
		ret.ActualValue = string(ev)
		return ret
	case t.BOOL:
		ev, _ := value.(typeDefs.ElBool)
		ret.ActualValue = bool(ev)
		return ret
	}
	util.Fatalf("Unknown primitive type: %s\n", t.TYPES[kind])
	return nil
}

func decodeComposite(kind int, value interface{}) interface{} {
	ret := new(parser.ParseTree)
	ret.ActualType = kind
	switch kind {
	case t.TUPLE:
		ev, _ := value.(typeDefs.ElTuple)
		tuple := new(t.TupleRep)
		tuple.Len = len(ev.Content)
		tuple.Type = int(ev.Type)
		for _, v := range ev.Content {
			var decoded interface{}
			if !t.IsComposite(tuple.Type) {
				decoded = decodePrimitive_(tuple.Type, v)
			} else {
				decoded = decodeComposite(tuple.Type, v)
			}
			tuple.Content = append(tuple.Content, decoded)
		}
		ret.ActualValue = tuple
		return ret
	}
	util.Fatalf("Unknown composite type: %s\n", t.TYPES[kind])
	return nil
}
