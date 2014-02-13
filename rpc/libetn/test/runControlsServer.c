// NOTE: Modified from source generated by rpcgen.

#include <rpc/rpc.h>

#include "controlTypes.h"

int _rpcpmstart = 0; // Not started by inetd.

void certificateprog_1(struct svc_req *rqstp, register SVCXPRT *transp);

uint32_t *
senduint32_1_svc(uint32_t *argp, struct svc_req *rqstp)
{
	return argp;
}

quad_t *
senduint64_1_svc(quad_t *argp, struct svc_req *rqstp)
{
	return argp;
}

float *
sendfloat32_1_svc(float *argp, struct svc_req *rqstp)
{
	return argp;
}

double *
sendfloat64_1_svc(double *argp, struct svc_req *rqstp)
{
	return argp;
}

u_int *
senduint32array_1_svc(u_int *argp, struct svc_req *rqstp)
{
	return argp;
}

IdentityCertificate *
sendcertificate_1_svc(IdentityCertificate *argp, struct svc_req *rqstp)
{
	return argp;
}

Torture *
sendtorture_1_svc(Torture *argp, struct svc_req *rqstp)
{
	return argp;
}

TreeNode *
sendtreenode_1_svc(TreeNode *argp, struct svc_req *rqstp)
{
	return argp;
}

char **
sendstring_1_svc(char **argp, struct svc_req *rqstp)
{
	return argp;
}

int
main (int argc, char **argv)
{
	pmap_unset (CERTIFICATEPROG, CERTIFICATEVERS);

	if (! svc_create(certificateprog_1, CERTIFICATEPROG, CERTIFICATEVERS, "netpath")) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
}
