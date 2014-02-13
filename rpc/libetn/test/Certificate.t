HashValue [64]byte
PublicSignatureKey   [32]byte
Signature   [128]byte
PublicEncryptionKey [32]byte
IpAddress   uint32
UdpPort     uint16

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

IdentityCertificate struct {
        Header CertificateHeader
	Subject string
	PublicSignatureKey [32]byte
}

DirectoryServiceRecord struct {
        Header             CertificateHeader
        Protocol           Uint32
        Hostname           String
        PublicKey          PublicEncryptionKey
        EphemeralPublicKey PublicEncryptionKey
        IpAddress          IpAddress
        UdpPort            UdpPort
        Padding            Uint32
}
