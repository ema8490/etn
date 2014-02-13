RpcMinimaLT interface {
	// Kernel-to-kernel RPCs.
	ConnectionCreate (cid uint64, connectionId uint32, service string, typeHash HashValue, writeValue string) (rcid uint64, status Status)
	ConnectionCreateAuthenticateUser (cid uint64, connectionId uint32, service string, typeHash HashValue, userPublicKey PublicEncryptionKey, userAuthenticator Authenticator, writeValue string) (rcid uint64, status Status)
	ConnectionClose (cid uint64, connectionId uint32) (rcid uint64, status Status)
	NextTid (cid uint64, tid uint64, key PublicEncryptionKey) (rcid uint64, status Status)
	RekeyNow (cid uint64) (rcid uint64, status Status)
	Puzzle (cid uint64, puzzle Puzzle) (rcid uint64, puzzleSolution PuzzleSolution, status Status)
	ConnectionWindowSize (cid uint64, connectionId uint32, size uint64) (rcid uint64, status Status)

	// TODO: Remove is favor of using application-specific RPCs?
	IpcWrite (cid uint64, contents string) (rcid uint64, status Status)

	// Kernel-to-Kernel service RPC, used only for T1 tunnel
	DirectoryServiceLookup (cid uint64, hostname string) (rcid uint64, record DirectoryServiceRecord, status Status)
}
