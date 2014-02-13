Authenticator struct {
	Nonce uint64
	//   crypto_box_ZEROBYTES (32).
	// + Recipient public encryption key size (32)
	Authenticator [64]byte
}
