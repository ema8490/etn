CertificateHeader struct {
	Size      uint32
	Version   uint32
	ValidFrom [16]byte
	ValidTo   [16]byte
	TypeHash  HashValue
	PublicSignatureKey PublicSignatureKey
	// RFC 1034 + terminating NULL.
	RevocationServer [256]byte
	// Hash size (64) + crypto signature size (64).
	Signature Signature
	// For SayI revocation; coult use hash over
	// certificate, but this would increase bandwidth.
	SerialNum  uint32
}
