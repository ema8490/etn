// FIXME: unify behavior of libetn and Go.
// Go does not create pointer types automatically.
GoUint8Ptr *uint8

RpcBenchmark interface {
	SendBool (cid uint64, args bool) (rcid uint64, ret bool)
	SendUint8 (cid uint64, args uint8) (rcid uint64, ret uint8)
	SendUint16 (cid uint64, args uint16) (rcid uint64, ret uint16)
	SendUint32 (cid uint64, args uint32) (rcid uint64, ret uint32)
	SendUint64 (cid uint64, args uint64) (rcid uint64, ret uint64)
	SendFloat32 (cid uint64, args float32) (rcid uint64, ret float32)
	SendFloat64 (cid uint64, args float64) (rcid uint64, ret float64)
	SendString (cid uint64, args string) (rcid uint64, ret string)
	SendByteArray (cid uint64, args ByteArray) (rcid uint64, ret ByteArray)
	SendUint32Array (cid uint64, args Uint32Array) (rcid uint64, ret Uint32Array)
	SendCertificate (cid uint64, args IdentityCertificate) (rcid uint64, ret IdentityCertificate)
	SendTorture (cid uint64, args Torture) (rcid uint64, ret Torture)
	SendTreeNode (cid uint64, args TreeNode) (rcid uint64, ret TreeNode)
}
