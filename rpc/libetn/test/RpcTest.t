RpcTest interface {
	Hello (cid uint64) (rcid uint64)
	Sprintf (cid uint64, msg string) (rcid uint64, fnval string)
	TwoArgsOneReturn (cid uint64, arg1, arg2 string) (rcid uint64, fnval string)
	OneArgTwoReturns (cid uint64, arg string) (rcid uint64, fnval1, fnval2 string)
	ArrayArg (cid uint64, arg ByteArray) (rcid uint64, fnval ByteArray)
	TupleArg (cid uint64, connectionId uint32, value []byte) (rcid uint64, status uint64, eventId uint64)
}
