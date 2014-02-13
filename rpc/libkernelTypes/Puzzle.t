Puzzle struct {
	// 88 is for checksum, client eph. key, server eph. key, nonce
	EncryptedData [88]byte
	Hash          HashValue
	MissingBits   uint32
	Nonce         uint64
}

PuzzleSolution struct {
	EncryptedData [88]byte
	Nonce         uint64
}

PuzzleSecret struct {
	Nonce              uint64
	ClientEphemeralKey PublicEncryptionKey
	ServerEphemeralKey PublicEncryptionKey
}
