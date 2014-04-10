package io

import (
	"ethos/msgTypes"
	"ethos/typeDefs"
	"reflect"
)

type MappingType struct {
	Type       reflect.Type
	NewEncoder reflect.Value
	NewDecoder reflect.Value
}

var Mapping = map[string]*MappingType{}

func Init() {
	Mapping["Message"] = &MappingType{Type: reflect.TypeOf(msgTypes.Message{}), NewEncoder: reflect.ValueOf(msgTypes.NewEncoder), NewDecoder: reflect.ValueOf(msgTypes.NewDecoder)}
	Mapping["Mailbox"] = &MappingType{Type: reflect.TypeOf(msgTypes.Mailbox{}), NewEncoder: reflect.ValueOf(msgTypes.NewEncoder), NewDecoder: reflect.ValueOf(msgTypes.NewDecoder)}
	// Mapping["TestPrimitive"] = &MappingType{Type: reflect.TypeOf(t.TestPrimitive{}), NewEncoder: reflect.ValueOf(t.NewEncoder)}
	// Mapping["TestComposite"] = &MappingType{Type: reflect.TypeOf(t.TestComposite{}), NewEncoder: reflect.ValueOf(t.NewEncoder)}
	Mapping["TestTuple"] = &MappingType{Type: reflect.TypeOf(typeDefs.TestTuple{}), NewEncoder: reflect.ValueOf(typeDefs.NewEncoder), NewDecoder: reflect.ValueOf(typeDefs.NewDecoder)}
	Mapping["TestMixed"] = &MappingType{Type: reflect.TypeOf(typeDefs.TestMixed{}), NewEncoder: reflect.ValueOf(typeDefs.NewEncoder), NewDecoder: reflect.ValueOf(typeDefs.NewDecoder)}
}
