const DATESIZE = 16;
const HASHSIZE = 64;
const PUBKEYSIZE = 32;
const HOSTSIZE = 256;
const SIGSIZE = 128;

typedef unsigned int       Uint32;
typedef unsigned hyper int Uint64;
typedef float              Float32;
typedef double             Float64;
typedef unsigned int      *UintPtr;
typedef unsigned int       Uint32Array[1024];
typedef string             String<>;

struct CertificateHeader {
	unsigned int size;
	unsigned int ver;
	char         validFrom[DATESIZE];
	char         validTo[DATESIZE];
	char         typeHash[HASHSIZE];
	char         publicSignatureKey[PUBKEYSIZE];
	char         host[HOSTSIZE];
	char         signature[SIGSIZE];
};

struct IdentityCertificate {
	CertificateHeader header;
	string            subject<>;
	char              publicSignatureKey[PUBKEYSIZE];
};

struct Torture {
	unsigned int       i1;     /* Simulate int8, uint8, int16. */
	int                i2;     /* Actually should be uint16 (result is two extra bytes). */
	int                i3;
	unsigned int       i4;
	hyper int          i5;
	unsigned hyper int i6;
	unsigned int      *p;      /* Actually should be byte pointer. */
	string             t<>;
	unsigned int       a[256]; /* Actually 1024 bytes. */
};

struct TreeNode {
	TreeNode *left;
	TreeNode *right;
	int       id;
};

program CERTIFICATEPROG {
	version CERTIFICATEVERS {
		unsigned int        SENDUINT32(unsigned int) = 1;
		hyper               SENDUINT64(hyper) = 2;
		float               SENDFLOAT32(float) = 3;
		double              SENDFLOAT64(double) = 4;
		string              SENDSTRING(string) = 5;
		Uint32Array         SENDUINT32ARRAY(Uint32Array) = 6;
		IdentityCertificate SENDCERTIFICATE(IdentityCertificate) = 7;
		Torture             SENDTORTURE(Torture) = 8;
		TreeNode            SENDTREENODE(TreeNode) = 9;
	} = 1;
} = 0xdeadbeef;
