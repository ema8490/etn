#include <ethos/dual/ref.h>
#include <ethos/generic/slice.h>
#include <ethos/generic/types.h>
#include <stdint.h>

// NOTE: Must match types.go type constants.
int etnScalarSizeMap[] = { 1, 1, 1, 2, 2, 4, 4, 8, 8, 4, 8 };

EtnType Int8Type = {EtnKindInt8, 1, (EtnMethods){0, 0}};
EtnType Uint8Type = {EtnKindUint8, 1, (EtnMethods){0, 0}};
EtnType Int16Type = {EtnKindInt16, 2, (EtnMethods){0, 0}};
EtnType Uint16Type = {EtnKindUint16, 2, (EtnMethods){0, 0}};
EtnType Int32Type = {EtnKindInt32, 4, (EtnMethods){0, 0}};
EtnType Uint32Type = {EtnKindUint32, 4, (EtnMethods){0, 0}};
EtnType Int64Type = {EtnKindInt64, 8, (EtnMethods){0, 0}};
EtnType Uint64Type = {EtnKindUint64, 8, (EtnMethods){0, 0}};
EtnType Float32Type = {EtnKindFloat32, 4, (EtnMethods){0, 0}};
EtnType Float64Type = {EtnKindFloat64, 8, (EtnMethods){0, 0}};
EtnType BoolType = {EtnKindBool, 1, (EtnMethods){0, 0}};
EtnType AnyType = {EtnKindAny, sizeof(EtnValue), (EtnMethods){0, 0}};

EtnIndirectType StringType = {(EtnType){.kind=EtnKindString, .size=sizeof(String *)}, &Uint8Type};

// Aliases.
EtnType PtrEncodingType = {EtnKindUint8, 1, (EtnMethods){0, 0}};
EtnType LengthType = {EtnKindUint32, 4, (EtnMethods){0, 0}};
EtnType CallIdType = {EtnKindUint64, 8, (EtnMethods){0, 0}};
EtnType UnionIdType = {EtnKindUint64, 8, (EtnMethods){0, 0}};

// Aliases.
EtnIndirectType HashType = {(EtnType){.kind=EtnKindString, .size=sizeof(String *)}, &Uint8Type};
