RpcTerminal interface {
	Ping (cid uint64) (rcid uint64, status Status)
	Write (cid uint64, msg string) (rcid uint64, status Status)
	Read (cid uint64) (rcid uint64, msg string, status Status)
}
