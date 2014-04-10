package ev

import (
	"ethos/elTypes"
	"ethos/environment"
	"ethos/io"
	"ethos/operators"
	"ethos/parser"
	"ethos/util"
	"fmt"
	"reflect"
)

func constructorUp(node *parser.ParseTree, env *en.StandardEnvironment) bool {
	typeName := string(node.Children[0].Value)

	if io.IsPrimitive(typeName) {
		util.Printf("%s is primitive\n")
		return false
	}

	at, av := constructor(typeName, node.Children[1].Children)
	node.ActualType = at
	if av != nil {
		node.ActualValue = av
	}

	return false
}

func constructor(typeName string, fields []*parser.ParseTree) (at int, av interface{}) {
	if io.Mapping[typeName] == nil {
		util.Printf("unknonwn type %s\n", typeName)
		return t.UNDEFINED, nil
	}

	o := reflect.New(io.Mapping[typeName].Type)

	switch o.Elem().Kind() {
	case reflect.Struct:
		return structConstructor(typeName, o, fields)
	case reflect.Array, reflect.Slice:
		return sliceConstructor(typeName, o, fields)
	default:
		util.Printf("unknown kind %s\n", o.Kind().String())
	}

	return t.UNDEFINED, nil
}

func structConstructor(typeName string, o reflect.Value, fields []*parser.ParseTree) (at int, av interface{}) {
	for _, field := range fields {
		if len(field.Children) != 2 {
			util.Printf("bad constructor syntax for a struct kind\n")
			return t.UNDEFINED, nil
		}

		name := string(field.Children[0].Value)
		if !StructHasField(o.Elem(), name) {
			util.Printf("missing field %s.%s\n", typeName, name)
			return t.UNDEFINED, nil
		}

		if field.Children[1].ActualType == t.UNDEFINED {
			op.UnexpectedType(field.Children[1])
			return t.UNDEFINED, nil
		}

		switch v := field.Children[1].ActualValue.(type) {
		case reflect.Value:
			o.Elem().FieldByName(name).Set(v)
		default:
			o.Elem().FieldByName(name).Set(reflect.ValueOf(v))
		}
	}

	return t.OBJECT, o.Elem()
}

func sliceConstructor(typeName string, o reflect.Value, fields []*parser.ParseTree) (at int, av interface{}) {
	o = o.Elem()
	for _, field := range fields {
		if field.ActualType == t.UNDEFINED {
			op.UnexpectedType(field)
			return t.UNDEFINED, nil
		}

		switch v := field.ActualValue.(type) {
		case reflect.Value:
			o = reflect.Append(o, v)
		default:
			o = reflect.Append(o, reflect.ValueOf(v))
		}
	}

	return t.OBJECT, o
}

func StructHasField(s reflect.Value, field string) bool {
	for i := 0; i < s.NumField(); i++ {
		if s.Type().Field(i).Name == field {
			return true
		}
	}
	return false
}

func printFields(fieldName, typeName string, ind int) {
	s := ""
	for i := 0; i < ind; i++ {
		s += "  "
	}
	fmt.Printf("%s%s: %s", s, fieldName, typeName)

	if io.IsPrimitive(typeName) {
		fmt.Print(" (primitive)\n")
		return
	}

	fmt.Println("")

	fields := io.GetTypeFields(typeName)
	if fields == nil {
		fmt.Printf("%snot found\n", s)
		return
	}

	for k, t := range fields {
		printFields(k, t, ind+1)
	}
}

func accessStruct(node, value, access *parser.ParseTree, env *en.StandardEnvironment) bool {
	o, ok := value.ActualValue.(reflect.Value)

	if !ok {
		return true
	}

	Eval(access, env)

	field := string(access.Value)

	if !StructHasField(o, field) {
		util.Printf("missing field %s\n", field)
		return true
	}

	v := o.FieldByName(field)
	typeName := io.SimpleTypeName(v)

	if io.IsPrimitive(typeName) {
		at := t.PrimitiveTypeFromBase(io.GetTypeBase(typeName))

		av := io.CastPrimitive(at, v)
		if av != nil {
			node.ActualType = at
			node.ActualValue = av
		}
	} else {
		node.ActualType = t.OBJECT
		node.ActualValue = v
	}

	return true
}

func accessSlice(node *parser.ParseTree, index int) (interface{}, int) {
	o, ok := node.ActualValue.(reflect.Value)
	if !ok {
		op.UnexpectedType(node)
		return nil, t.UNDEFINED
	}

	k := o.Kind()
	if k != reflect.Slice && k != reflect.Array {
		op.UnexpectedType(node)
		return nil, t.UNDEFINED
	}

	if index < 0 || index >= o.Len() {
		indexOutOfRange(node)
		return nil, t.UNDEFINED
	}

	element := o.Index(index)
	typeName := io.SimpleTypeName(element)
	util.Printf("%s\n", typeName)

	var at int
	var av interface{}
	if io.IsPrimitive(typeName) {
		at = t.PrimitiveTypeFromBase(io.GetTypeBase(typeName))

		av = io.CastPrimitive(at, element)
		if av != nil {
			node.ActualType = at
			node.ActualValue = av
		}
	} else {
		at = t.OBJECT
		av = element
	}

	return av, at
}

func lenSlice(value interface{}) int {
	o, ok := value.(reflect.Value)
	if !ok {
		return -1
	}

	k := o.Kind()
	if k != reflect.Slice && k != reflect.Array {
		return -1
	}

	return o.Len()
}
