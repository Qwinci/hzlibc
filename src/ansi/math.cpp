#include "math.h"
#include "utils.hpp"
#include "limits.h"
#include <hz/bit.hpp>

// --------- binary32 ---------
// 1bit sign
// 8bit exponent
// 24bit significant (last bit implicitly stored as 1)

// exponent 1 == -126
// exponent 127 == 0
// exponent 254 == 127

// add values for bits set in significant together
// bit 23 = 1 (implicit bit)
// bit 22 = 0.5
// bit 21 = 0.25
// ...
// bit 0 = pow(2, -23)
// + multiply by pow(2, exp - 127)
// + multiply by sign

// --------- binary64 ---------
// 1bit sign
// 11bit exponent
// 53bit significant (last bit implicitly stored as 1)

// exponent 1 == -1022
// exponent 1023 == 0
// exponent 2046 == 1023

// add values for bits set in significant together
// bit 52 = 1 (implicit bit)
// bit 51 = 0.5
// bit 50 = 0.25
// ...
// bit 0 = pow(2, -52)
// + multiply by pow(2, exp - 1023)
// + multiply by sign

EXPORT float powf(float x, float y) {
	//__ensure(!"powf is not implemented");
	int value = static_cast<int>(y);
	auto start = x;
	for (int i = 1; i < value; ++i) {
		x += start;
	}
	return x;
}

EXPORT double pow(double x, double y) {
	//__ensure(!"pow is not implemented");
	auto value = static_cast<long long>(y);
	auto start = x;
	for (int i = 1; i < value; ++i) {
		x += start;
	}
	return x;
}

EXPORT float sqrtf(float x) {
	__ensure(!"sqrtf is not implemented");
}

EXPORT double sqrt(double x) {
	__ensure(!"sqrt is not implemented");
}

const float s_log_C0 = -19.645704f;
const float s_log_C1 = 0.767002f;
const float s_log_C2 = 0.3717479f;
const float s_log_C3 = 5.2653985f;
const float s_log_C4 = -(1.0f + s_log_C0) * (1.0f + s_log_C1) / ((1.0f + s_log_C2) * (1.0f + s_log_C3)); //ensures that log(1) == 0
const float s_log_2 = 0.6931472f;

#define WANT_ROUNDING 1
#define T __logf_data.tab
#define A __logf_data.poly
#define Ln2 __logf_data.ln2
#define N (1 << LOGF_TABLE_BITS)
#define OFF 0x3f330000

#define LOGF_TABLE_BITS 4
#define LOGF_POLY_ORDER 4
struct logf_data {
	struct {
		double invc, logc;
	} tab[1 << LOGF_TABLE_BITS];
	double ln2;
	double poly[LOGF_POLY_ORDER - 1]; /* First order coefficient is 1.  */
};

const struct logf_data __logf_data = {
	.tab = {
		{ 0x1.661ec79f8f3bep+0, -0x1.57bf7808caadep-2 },
		{ 0x1.571ed4aaf883dp+0, -0x1.2bef0a7c06ddbp-2 },
		{ 0x1.49539f0f010bp+0, -0x1.01eae7f513a67p-2 },
		{ 0x1.3c995b0b80385p+0, -0x1.b31d8a68224e9p-3 },
		{ 0x1.30d190c8864a5p+0, -0x1.6574f0ac07758p-3 },
		{ 0x1.25e227b0b8eap+0, -0x1.1aa2bc79c81p-3 },
		{ 0x1.1bb4a4a1a343fp+0, -0x1.a4e76ce8c0e5ep-4 },
		{ 0x1.12358f08ae5bap+0, -0x1.1973c5a611cccp-4 },
		{ 0x1.0953f419900a7p+0, -0x1.252f438e10c1ep-5 },
		{ 0x1p+0, 0x0p+0 },
		{ 0x1.e608cfd9a47acp-1, 0x1.aa5aa5df25984p-5 },
		{ 0x1.ca4b31f026aap-1, 0x1.c5e53aa362eb4p-4 },
		{ 0x1.b2036576afce6p-1, 0x1.526e57720db08p-3 },
		{ 0x1.9c2d163a1aa2dp-1, 0x1.bc2860d22477p-3 },
		{ 0x1.886e6037841edp-1, 0x1.1058bc8a07ee1p-2 },
		{ 0x1.767dcf5534862p-1, 0x1.4043057b6ee09p-2 },
	},
	.ln2 = 0x1.62e42fefa39efp-1,
	.poly = {
		-0x1.00ea348b88334p-2, 0x1.5575b0be00b6ap-2, -0x1.ffffef20a4123p-2,
	}
};

#define asuint(f) hz::bit_cast<uint32_t>(f)
#define asfloat(i) hz::bit_cast<float>(i)
#define asuint64(f) hz::bit_cast<uint64_t>(f)
#define asdouble(i) hz::bit_cast<double>(i)

static inline float eval_as_float(float x)
{
	float y = x;
	return y;
}

static inline double eval_as_double(double x)
{
	double y = x;
	return y;
}

EXPORT float logf(float x) {
	double z, r, r2, y, y0, invc, logc;
	uint32_t ix, iz, tmp;
	int k, i;

	ix = asuint(x);
	/* Fix sign of zero with downward rounding when x==1.  */
	if (WANT_ROUNDING && ix == 0x3f800000)
		return 0;
	if (ix - 0x00800000 >= 0x7f800000 - 0x00800000) {
		/* x < 0x1p-126 or inf or nan.  */
		if (ix * 2 == 0)
			return __builtin_nanf("");
		if (ix == 0x7f800000) /* log(inf) == inf.  */
			return x;
		if ((ix & 0x80000000) || ix * 2 >= 0xff000000)
			return __builtin_nanf("");
		/* x is subnormal, normalize it.  */
		ix = asuint(x * 0x1p23f);
		ix -= 23 << 23;
	}

	/* x = 2^k z; where z is in range [OFF,2*OFF] and exact.
	   The range is split into N subintervals.
	   The ith subinterval contains z and c is near its center.  */
	tmp = ix - OFF;
	i = (tmp >> (23 - LOGF_TABLE_BITS)) % N;
	k = (int32_t)tmp >> 23; /* arithmetic shift */
	iz = ix - (tmp & 0xff800000);
	invc = T[i].invc;
	logc = T[i].logc;
	z = (double)asfloat(iz);

	/* log(x) = log1p(z/c-1) + log(c) + k*Ln2 */
	r = z * invc - 1;
	y0 = logc + (double)k * Ln2;

	/* Pipelined polynomial evaluation to approximate log1p(r).  */
	r2 = r * r;
	y = A[1] * r + A[2];
	y = A[0] * r2 + y;
	y = y * r2 + (y0 + r);
	return eval_as_float(y);
}

#define N (1 << LOG_TABLE_BITS)

#define LOG_TABLE_BITS 7
#define LOG_POLY_ORDER 6
#define LOG_POLY1_ORDER 12
struct log_data {
	double ln2hi;
	double ln2lo;
	double poly[LOG_POLY_ORDER - 1]; /* First coefficient is 1.  */
	double poly1[LOG_POLY1_ORDER - 1];
	struct {
		double invc, logc;
	} tab[1 << LOG_TABLE_BITS];
#if !__FP_FAST_FMA
	struct {
		double chi, clo;
	} tab2[1 << LOG_TABLE_BITS];
#endif
};

const struct log_data __log_data = {
	.ln2hi = 0x1.62e42fefa3800p-1,
	.ln2lo = 0x1.ef35793c76730p-45,
	.poly = {
// relative error: 0x1.926199e8p-56
// abs error: 0x1.882ff33p-65
// in -0x1.fp-9 0x1.fp-9
		-0x1.0000000000001p-1,
		0x1.555555551305bp-2,
		-0x1.fffffffeb459p-3,
		0x1.999b324f10111p-3,
		-0x1.55575e506c89fp-3,
	},
	.poly1 = {
// relative error: 0x1.c04d76cp-63
// in -0x1p-4 0x1.09p-4 (|log(1+x)| > 0x1p-4 outside the interval)
		-0x1p-1,
		0x1.5555555555577p-2,
		-0x1.ffffffffffdcbp-3,
		0x1.999999995dd0cp-3,
		-0x1.55555556745a7p-3,
		0x1.24924a344de3p-3,
		-0x1.fffffa4423d65p-4,
		0x1.c7184282ad6cap-4,
		-0x1.999eb43b068ffp-4,
		0x1.78182f7afd085p-4,
		-0x1.5521375d145cdp-4,
	},
/* Algorithm:

	x = 2^k z
	log(x) = k ln2 + log(c) + log(z/c)
	log(z/c) = poly(z/c - 1)

where z is in [1.6p-1; 1.6p0] which is split into N subintervals and z falls
into the ith one, then table entries are computed as

	tab[i].invc = 1/c
	tab[i].logc = (double)log(c)
	tab2[i].chi = (double)c
	tab2[i].clo = (double)(c - (double)c)

where c is near the center of the subinterval and is chosen by trying +-2^29
floating point invc candidates around 1/center and selecting one for which

	1) the rounding error in 0x1.8p9 + logc is 0,
	2) the rounding error in z - chi - clo is < 0x1p-66 and
	3) the rounding error in (double)log(c) is minimized (< 0x1p-66).

Note: 1) ensures that k*ln2hi + logc can be computed without rounding error,
2) ensures that z/c - 1 can be computed as (z - chi - clo)*invc with close to
a single rounding error when there is no fast fma for z*invc - 1, 3) ensures
that logc + poly(z/c - 1) has small error, however near x == 1 when
|log(x)| < 0x1p-4, this is not enough so that is special cased.  */
	.tab = {
		{0x1.734f0c3e0de9fp+0, -0x1.7cc7f79e69000p-2},
		{0x1.713786a2ce91fp+0, -0x1.76feec20d0000p-2},
		{0x1.6f26008fab5a0p+0, -0x1.713e31351e000p-2},
		{0x1.6d1a61f138c7dp+0, -0x1.6b85b38287800p-2},
		{0x1.6b1490bc5b4d1p+0, -0x1.65d5590807800p-2},
		{0x1.69147332f0cbap+0, -0x1.602d076180000p-2},
		{0x1.6719f18224223p+0, -0x1.5a8ca86909000p-2},
		{0x1.6524f99a51ed9p+0, -0x1.54f4356035000p-2},
		{0x1.63356aa8f24c4p+0, -0x1.4f637c36b4000p-2},
		{0x1.614b36b9ddc14p+0, -0x1.49da7fda85000p-2},
		{0x1.5f66452c65c4cp+0, -0x1.445923989a800p-2},
		{0x1.5d867b5912c4fp+0, -0x1.3edf439b0b800p-2},
		{0x1.5babccb5b90dep+0, -0x1.396ce448f7000p-2},
		{0x1.59d61f2d91a78p+0, -0x1.3401e17bda000p-2},
		{0x1.5805612465687p+0, -0x1.2e9e2ef468000p-2},
		{0x1.56397cee76bd3p+0, -0x1.2941b3830e000p-2},
		{0x1.54725e2a77f93p+0, -0x1.23ec58cda8800p-2},
		{0x1.52aff42064583p+0, -0x1.1e9e129279000p-2},
		{0x1.50f22dbb2bddfp+0, -0x1.1956d2b48f800p-2},
		{0x1.4f38f4734ded7p+0, -0x1.141679ab9f800p-2},
		{0x1.4d843cfde2840p+0, -0x1.0edd094ef9800p-2},
		{0x1.4bd3ec078a3c8p+0, -0x1.09aa518db1000p-2},
		{0x1.4a27fc3e0258ap+0, -0x1.047e65263b800p-2},
		{0x1.4880524d48434p+0, -0x1.feb224586f000p-3},
		{0x1.46dce1b192d0bp+0, -0x1.f474a7517b000p-3},
		{0x1.453d9d3391854p+0, -0x1.ea4443d103000p-3},
		{0x1.43a2744b4845ap+0, -0x1.e020d44e9b000p-3},
		{0x1.420b54115f8fbp+0, -0x1.d60a22977f000p-3},
		{0x1.40782da3ef4b1p+0, -0x1.cc00104959000p-3},
		{0x1.3ee8f5d57fe8fp+0, -0x1.c202956891000p-3},
		{0x1.3d5d9a00b4ce9p+0, -0x1.b81178d811000p-3},
		{0x1.3bd60c010c12bp+0, -0x1.ae2c9ccd3d000p-3},
		{0x1.3a5242b75dab8p+0, -0x1.a45402e129000p-3},
		{0x1.38d22cd9fd002p+0, -0x1.9a877681df000p-3},
		{0x1.3755bc5847a1cp+0, -0x1.90c6d69483000p-3},
		{0x1.35dce49ad36e2p+0, -0x1.87120a645c000p-3},
		{0x1.34679984dd440p+0, -0x1.7d68fb4143000p-3},
		{0x1.32f5cceffcb24p+0, -0x1.73cb83c627000p-3},
		{0x1.3187775a10d49p+0, -0x1.6a39a9b376000p-3},
		{0x1.301c8373e3990p+0, -0x1.60b3154b7a000p-3},
		{0x1.2eb4ebb95f841p+0, -0x1.5737d76243000p-3},
		{0x1.2d50a0219a9d1p+0, -0x1.4dc7b8fc23000p-3},
		{0x1.2bef9a8b7fd2ap+0, -0x1.4462c51d20000p-3},
		{0x1.2a91c7a0c1babp+0, -0x1.3b08abc830000p-3},
		{0x1.293726014b530p+0, -0x1.31b996b490000p-3},
		{0x1.27dfa5757a1f5p+0, -0x1.2875490a44000p-3},
		{0x1.268b39b1d3bbfp+0, -0x1.1f3b9f879a000p-3},
		{0x1.2539d838ff5bdp+0, -0x1.160c8252ca000p-3},
		{0x1.23eb7aac9083bp+0, -0x1.0ce7f57f72000p-3},
		{0x1.22a012ba940b6p+0, -0x1.03cdc49fea000p-3},
		{0x1.2157996cc4132p+0, -0x1.f57bdbc4b8000p-4},
		{0x1.201201dd2fc9bp+0, -0x1.e370896404000p-4},
		{0x1.1ecf4494d480bp+0, -0x1.d17983ef94000p-4},
		{0x1.1d8f5528f6569p+0, -0x1.bf9674ed8a000p-4},
		{0x1.1c52311577e7cp+0, -0x1.adc79202f6000p-4},
		{0x1.1b17c74cb26e9p+0, -0x1.9c0c3e7288000p-4},
		{0x1.19e010c2c1ab6p+0, -0x1.8a646b372c000p-4},
		{0x1.18ab07bb670bdp+0, -0x1.78d01b3ac0000p-4},
		{0x1.1778a25efbcb6p+0, -0x1.674f145380000p-4},
		{0x1.1648d354c31dap+0, -0x1.55e0e6d878000p-4},
		{0x1.151b990275fddp+0, -0x1.4485cdea1e000p-4},
		{0x1.13f0ea432d24cp+0, -0x1.333d94d6aa000p-4},
		{0x1.12c8b7210f9dap+0, -0x1.22079f8c56000p-4},
		{0x1.11a3028ecb531p+0, -0x1.10e4698622000p-4},
		{0x1.107fbda8434afp+0, -0x1.ffa6c6ad20000p-5},
		{0x1.0f5ee0f4e6bb3p+0, -0x1.dda8d4a774000p-5},
		{0x1.0e4065d2a9fcep+0, -0x1.bbcece4850000p-5},
		{0x1.0d244632ca521p+0, -0x1.9a1894012c000p-5},
		{0x1.0c0a77ce2981ap+0, -0x1.788583302c000p-5},
		{0x1.0af2f83c636d1p+0, -0x1.5715e67d68000p-5},
		{0x1.09ddb98a01339p+0, -0x1.35c8a49658000p-5},
		{0x1.08cabaf52e7dfp+0, -0x1.149e364154000p-5},
		{0x1.07b9f2f4e28fbp+0, -0x1.e72c082eb8000p-6},
		{0x1.06ab58c358f19p+0, -0x1.a55f152528000p-6},
		{0x1.059eea5ecf92cp+0, -0x1.63d62cf818000p-6},
		{0x1.04949cdd12c90p+0, -0x1.228fb8caa0000p-6},
		{0x1.038c6c6f0ada9p+0, -0x1.c317b20f90000p-7},
		{0x1.02865137932a9p+0, -0x1.419355daa0000p-7},
		{0x1.0182427ea7348p+0, -0x1.81203c2ec0000p-8},
		{0x1.008040614b195p+0, -0x1.0040979240000p-9},
		{0x1.fe01ff726fa1ap-1, 0x1.feff384900000p-9},
		{0x1.fa11cc261ea74p-1, 0x1.7dc41353d0000p-7},
		{0x1.f6310b081992ep-1, 0x1.3cea3c4c28000p-6},
		{0x1.f25f63ceeadcdp-1, 0x1.b9fc114890000p-6},
		{0x1.ee9c8039113e7p-1, 0x1.1b0d8ce110000p-5},
		{0x1.eae8078cbb1abp-1, 0x1.58a5bd001c000p-5},
		{0x1.e741aa29d0c9bp-1, 0x1.95c8340d88000p-5},
		{0x1.e3a91830a99b5p-1, 0x1.d276aef578000p-5},
		{0x1.e01e009609a56p-1, 0x1.07598e598c000p-4},
		{0x1.dca01e577bb98p-1, 0x1.253f5e30d2000p-4},
		{0x1.d92f20b7c9103p-1, 0x1.42edd8b380000p-4},
		{0x1.d5cac66fb5ccep-1, 0x1.606598757c000p-4},
		{0x1.d272caa5ede9dp-1, 0x1.7da76356a0000p-4},
		{0x1.cf26e3e6b2ccdp-1, 0x1.9ab434e1c6000p-4},
		{0x1.cbe6da2a77902p-1, 0x1.b78c7bb0d6000p-4},
		{0x1.c8b266d37086dp-1, 0x1.d431332e72000p-4},
		{0x1.c5894bd5d5804p-1, 0x1.f0a3171de6000p-4},
		{0x1.c26b533bb9f8cp-1, 0x1.067152b914000p-3},
		{0x1.bf583eeece73fp-1, 0x1.147858292b000p-3},
		{0x1.bc4fd75db96c1p-1, 0x1.2266ecdca3000p-3},
		{0x1.b951e0c864a28p-1, 0x1.303d7a6c55000p-3},
		{0x1.b65e2c5ef3e2cp-1, 0x1.3dfc33c331000p-3},
		{0x1.b374867c9888bp-1, 0x1.4ba366b7a8000p-3},
		{0x1.b094b211d304ap-1, 0x1.5933928d1f000p-3},
		{0x1.adbe885f2ef7ep-1, 0x1.66acd2418f000p-3},
		{0x1.aaf1d31603da2p-1, 0x1.740f8ec669000p-3},
		{0x1.a82e63fd358a7p-1, 0x1.815c0f51af000p-3},
		{0x1.a5740ef09738bp-1, 0x1.8e92954f68000p-3},
		{0x1.a2c2a90ab4b27p-1, 0x1.9bb3602f84000p-3},
		{0x1.a01a01393f2d1p-1, 0x1.a8bed1c2c0000p-3},
		{0x1.9d79f24db3c1bp-1, 0x1.b5b515c01d000p-3},
		{0x1.9ae2505c7b190p-1, 0x1.c2967ccbcc000p-3},
		{0x1.9852ef297ce2fp-1, 0x1.cf635d5486000p-3},
		{0x1.95cbaeea44b75p-1, 0x1.dc1bd3446c000p-3},
		{0x1.934c69de74838p-1, 0x1.e8c01b8cfe000p-3},
		{0x1.90d4f2f6752e6p-1, 0x1.f5509c0179000p-3},
		{0x1.8e6528effd79dp-1, 0x1.00e6c121fb800p-2},
		{0x1.8bfce9fcc007cp-1, 0x1.071b80e93d000p-2},
		{0x1.899c0dabec30ep-1, 0x1.0d46b9e867000p-2},
		{0x1.87427aa2317fbp-1, 0x1.13687334bd000p-2},
		{0x1.84f00acb39a08p-1, 0x1.1980d67234800p-2},
		{0x1.82a49e8653e55p-1, 0x1.1f8ffe0cc8000p-2},
		{0x1.8060195f40260p-1, 0x1.2595fd7636800p-2},
		{0x1.7e22563e0a329p-1, 0x1.2b9300914a800p-2},
		{0x1.7beb377dcb5adp-1, 0x1.3187210436000p-2},
		{0x1.79baa679725c2p-1, 0x1.377266dec1800p-2},
		{0x1.77907f2170657p-1, 0x1.3d54ffbaf3000p-2},
		{0x1.756cadbd6130cp-1, 0x1.432eee32fe000p-2},
	},
#if !__FP_FAST_FMA
	.tab2 = {
		{0x1.61000014fb66bp-1, 0x1.e026c91425b3cp-56},
		{0x1.63000034db495p-1, 0x1.dbfea48005d41p-55},
		{0x1.650000d94d478p-1, 0x1.e7fa786d6a5b7p-55},
		{0x1.67000074e6fadp-1, 0x1.1fcea6b54254cp-57},
		{0x1.68ffffedf0faep-1, -0x1.c7e274c590efdp-56},
		{0x1.6b0000763c5bcp-1, -0x1.ac16848dcda01p-55},
		{0x1.6d0001e5cc1f6p-1, 0x1.33f1c9d499311p-55},
		{0x1.6efffeb05f63ep-1, -0x1.e80041ae22d53p-56},
		{0x1.710000e86978p-1,  0x1.bff6671097952p-56},
		{0x1.72ffffc67e912p-1, 0x1.c00e226bd8724p-55},
		{0x1.74fffdf81116ap-1, -0x1.e02916ef101d2p-57},
		{0x1.770000f679c9p-1,  -0x1.7fc71cd549c74p-57},
		{0x1.78ffffa7ec835p-1, 0x1.1bec19ef50483p-55},
		{0x1.7affffe20c2e6p-1, -0x1.07e1729cc6465p-56},
		{0x1.7cfffed3fc9p-1,   -0x1.08072087b8b1cp-55},
		{0x1.7efffe9261a76p-1, 0x1.dc0286d9df9aep-55},
		{0x1.81000049ca3e8p-1, 0x1.97fd251e54c33p-55},
		{0x1.8300017932c8fp-1, -0x1.afee9b630f381p-55},
		{0x1.850000633739cp-1, 0x1.9bfbf6b6535bcp-55},
		{0x1.87000204289c6p-1, -0x1.bbf65f3117b75p-55},
		{0x1.88fffebf57904p-1, -0x1.9006ea23dcb57p-55},
		{0x1.8b00022bc04dfp-1, -0x1.d00df38e04b0ap-56},
		{0x1.8cfffe50c1b8ap-1, -0x1.8007146ff9f05p-55},
		{0x1.8effffc918e43p-1, 0x1.3817bd07a7038p-55},
		{0x1.910001efa5fc7p-1, 0x1.93e9176dfb403p-55},
		{0x1.9300013467bb9p-1, 0x1.f804e4b980276p-56},
		{0x1.94fffe6ee076fp-1, -0x1.f7ef0d9ff622ep-55},
		{0x1.96fffde3c12d1p-1, -0x1.082aa962638bap-56},
		{0x1.98ffff4458a0dp-1, -0x1.7801b9164a8efp-55},
		{0x1.9afffdd982e3ep-1, -0x1.740e08a5a9337p-55},
		{0x1.9cfffed49fb66p-1, 0x1.fce08c19bep-60},
		{0x1.9f00020f19c51p-1, -0x1.a3faa27885b0ap-55},
		{0x1.a10001145b006p-1, 0x1.4ff489958da56p-56},
		{0x1.a300007bbf6fap-1, 0x1.cbeab8a2b6d18p-55},
		{0x1.a500010971d79p-1, 0x1.8fecadd78793p-55},
		{0x1.a70001df52e48p-1, -0x1.f41763dd8abdbp-55},
		{0x1.a90001c593352p-1, -0x1.ebf0284c27612p-55},
		{0x1.ab0002a4f3e4bp-1, -0x1.9fd043cff3f5fp-57},
		{0x1.acfffd7ae1ed1p-1, -0x1.23ee7129070b4p-55},
		{0x1.aefffee510478p-1, 0x1.a063ee00edea3p-57},
		{0x1.b0fffdb650d5bp-1, 0x1.a06c8381f0ab9p-58},
		{0x1.b2ffffeaaca57p-1, -0x1.9011e74233c1dp-56},
		{0x1.b4fffd995badcp-1, -0x1.9ff1068862a9fp-56},
		{0x1.b7000249e659cp-1, 0x1.aff45d0864f3ep-55},
		{0x1.b8ffff987164p-1,  0x1.cfe7796c2c3f9p-56},
		{0x1.bafffd204cb4fp-1, -0x1.3ff27eef22bc4p-57},
		{0x1.bcfffd2415c45p-1, -0x1.cffb7ee3bea21p-57},
		{0x1.beffff86309dfp-1, -0x1.14103972e0b5cp-55},
		{0x1.c0fffe1b57653p-1, 0x1.bc16494b76a19p-55},
		{0x1.c2ffff1fa57e3p-1, -0x1.4feef8d30c6edp-57},
		{0x1.c4fffdcbfe424p-1, -0x1.43f68bcec4775p-55},
		{0x1.c6fffed54b9f7p-1, 0x1.47ea3f053e0ecp-55},
		{0x1.c8fffeb998fd5p-1, 0x1.383068df992f1p-56},
		{0x1.cb0002125219ap-1, -0x1.8fd8e64180e04p-57},
		{0x1.ccfffdd94469cp-1, 0x1.e7ebe1cc7ea72p-55},
		{0x1.cefffeafdc476p-1, 0x1.ebe39ad9f88fep-55},
		{0x1.d1000169af82bp-1, 0x1.57d91a8b95a71p-56},
		{0x1.d30000d0ff71dp-1, 0x1.9c1906970c7dap-55},
		{0x1.d4fffea790fc4p-1, -0x1.80e37c558fe0cp-58},
		{0x1.d70002edc87e5p-1, -0x1.f80d64dc10f44p-56},
		{0x1.d900021dc82aap-1, -0x1.47c8f94fd5c5cp-56},
		{0x1.dafffd86b0283p-1, 0x1.c7f1dc521617ep-55},
		{0x1.dd000296c4739p-1, 0x1.8019eb2ffb153p-55},
		{0x1.defffe54490f5p-1, 0x1.e00d2c652cc89p-57},
		{0x1.e0fffcdabf694p-1, -0x1.f8340202d69d2p-56},
		{0x1.e2fffdb52c8ddp-1, 0x1.b00c1ca1b0864p-56},
		{0x1.e4ffff24216efp-1, 0x1.2ffa8b094ab51p-56},
		{0x1.e6fffe88a5e11p-1, -0x1.7f673b1efbe59p-58},
		{0x1.e9000119eff0dp-1, -0x1.4808d5e0bc801p-55},
		{0x1.eafffdfa51744p-1, 0x1.80006d54320b5p-56},
		{0x1.ed0001a127fa1p-1, -0x1.002f860565c92p-58},
		{0x1.ef00007babcc4p-1, -0x1.540445d35e611p-55},
		{0x1.f0ffff57a8d02p-1, -0x1.ffb3139ef9105p-59},
		{0x1.f30001ee58ac7p-1, 0x1.a81acf2731155p-55},
		{0x1.f4ffff5823494p-1, 0x1.a3f41d4d7c743p-55},
		{0x1.f6ffffca94c6bp-1, -0x1.202f41c987875p-57},
		{0x1.f8fffe1f9c441p-1, 0x1.77dd1f477e74bp-56},
		{0x1.fafffd2e0e37ep-1, -0x1.f01199a7ca331p-57},
		{0x1.fd0001c77e49ep-1, 0x1.181ee4bceacb1p-56},
		{0x1.feffff7e0c331p-1, -0x1.e05370170875ap-57},
		{0x1.00ffff465606ep+0, -0x1.a7ead491c0adap-55},
		{0x1.02ffff3867a58p+0, -0x1.77f69c3fcb2ep-54},
		{0x1.04ffffdfc0d17p+0, 0x1.7bffe34cb945bp-54},
		{0x1.0700003cd4d82p+0, 0x1.20083c0e456cbp-55},
		{0x1.08ffff9f2cbe8p+0, -0x1.dffdfbe37751ap-57},
		{0x1.0b000010cda65p+0, -0x1.13f7faee626ebp-54},
		{0x1.0d00001a4d338p+0, 0x1.07dfa79489ff7p-55},
		{0x1.0effffadafdfdp+0, -0x1.7040570d66bcp-56},
		{0x1.110000bbafd96p+0, 0x1.e80d4846d0b62p-55},
		{0x1.12ffffae5f45dp+0, 0x1.dbffa64fd36efp-54},
		{0x1.150000dd59ad9p+0, 0x1.a0077701250aep-54},
		{0x1.170000f21559ap+0, 0x1.dfdf9e2e3deeep-55},
		{0x1.18ffffc275426p+0, 0x1.10030dc3b7273p-54},
		{0x1.1b000123d3c59p+0, 0x1.97f7980030188p-54},
		{0x1.1cffff8299eb7p+0, -0x1.5f932ab9f8c67p-57},
		{0x1.1effff48ad4p+0,   0x1.37fbf9da75bebp-54},
		{0x1.210000c8b86a4p+0, 0x1.f806b91fd5b22p-54},
		{0x1.2300003854303p+0, 0x1.3ffc2eb9fbf33p-54},
		{0x1.24fffffbcf684p+0, 0x1.601e77e2e2e72p-56},
		{0x1.26ffff52921d9p+0, 0x1.ffcbb767f0c61p-56},
		{0x1.2900014933a3cp+0, -0x1.202ca3c02412bp-56},
		{0x1.2b00014556313p+0, -0x1.2808233f21f02p-54},
		{0x1.2cfffebfe523bp+0, -0x1.8ff7e384fdcf2p-55},
		{0x1.2f0000bb8ad96p+0, -0x1.5ff51503041c5p-55},
		{0x1.30ffffb7ae2afp+0, -0x1.10071885e289dp-55},
		{0x1.32ffffeac5f7fp+0, -0x1.1ff5d3fb7b715p-54},
		{0x1.350000ca66756p+0, 0x1.57f82228b82bdp-54},
		{0x1.3700011fbf721p+0, 0x1.000bac40dd5ccp-55},
		{0x1.38ffff9592fb9p+0, -0x1.43f9d2db2a751p-54},
		{0x1.3b00004ddd242p+0, 0x1.57f6b707638e1p-55},
		{0x1.3cffff5b2c957p+0, 0x1.a023a10bf1231p-56},
		{0x1.3efffeab0b418p+0, 0x1.87f6d66b152bp-54},
		{0x1.410001532aff4p+0, 0x1.7f8375f198524p-57},
		{0x1.4300017478b29p+0, 0x1.301e672dc5143p-55},
		{0x1.44fffe795b463p+0, 0x1.9ff69b8b2895ap-55},
		{0x1.46fffe80475ep+0,  -0x1.5c0b19bc2f254p-54},
		{0x1.48fffef6fc1e7p+0, 0x1.b4009f23a2a72p-54},
		{0x1.4afffe5bea704p+0, -0x1.4ffb7bf0d7d45p-54},
		{0x1.4d000171027dep+0, -0x1.9c06471dc6a3dp-54},
		{0x1.4f0000ff03ee2p+0, 0x1.77f890b85531cp-54},
		{0x1.5100012dc4bd1p+0, 0x1.004657166a436p-57},
		{0x1.530001605277ap+0, -0x1.6bfcece233209p-54},
		{0x1.54fffecdb704cp+0, -0x1.902720505a1d7p-55},
		{0x1.56fffef5f54a9p+0, 0x1.bbfe60ec96412p-54},
		{0x1.5900017e61012p+0, 0x1.87ec581afef9p-55},
		{0x1.5b00003c93e92p+0, -0x1.f41080abf0ccp-54},
		{0x1.5d0001d4919bcp+0, -0x1.8812afb254729p-54},
		{0x1.5efffe7b87a89p+0, -0x1.47eb780ed6904p-54},
	},
#endif
};

#define T __log_data.tab
#define T2 __log_data.tab2
#define B __log_data.poly1
#define A __log_data.poly
#define Ln2hi __log_data.ln2hi
#define Ln2lo __log_data.ln2lo
#define N (1 << LOG_TABLE_BITS)
#define OFF 0x3fe6000000000000

/* Top 16 bits of a double.  */
static inline uint32_t top16(double x)
{
	return asuint64(x) >> 48;
}

EXPORT double log(double x) {
	double w, z, r, r2, r3, y, invc, logc, kd, hi, lo;
	uint64_t ix, iz, tmp;
	uint32_t top;
	int k, i;

	ix = asuint64(x);
	top = top16(x);
#define LO asuint64(1.0 - 0x1p-4)
#define HI asuint64(1.0 + 0x1.09p-4)
	if (ix - LO < HI - LO) {
		/* Handle close to 1.0 inputs separately.  */
		/* Fix sign of zero with downward rounding when x==1.  */
		if (WANT_ROUNDING && ix == asuint64(1.0))
			return 0;
		r = x - 1.0;
		r2 = r * r;
		r3 = r * r2;
		y = r3 *
		    (B[1] + r * B[2] + r2 * B[3] +
		     r3 * (B[4] + r * B[5] + r2 * B[6] +
		           r3 * (B[7] + r * B[8] + r2 * B[9] + r3 * B[10])));
		/* Worst-case error is around 0.507 ULP.  */
		w = r * 0x1p27;
		double rhi = r + w - w;
		double rlo = r - rhi;
		w = rhi * rhi * B[0]; /* B[0] == -0.5.  */
		hi = r + w;
		lo = r - hi + w;
		lo += B[0] * rlo * (rhi + r);
		y += lo;
		y += hi;
		return eval_as_double(y);
	}
	if (top - 0x0010 >= 0x7ff0 - 0x0010) {
		/* x < 0x1p-1022 or inf or nan.  */
		if (ix * 2 == 0)
			return __builtin_nan("");
		if (ix == asuint64(__builtin_inf())) /* log(inf) == inf.  */
			return x;
		if ((top & 0x8000) || (top & 0x7ff0) == 0x7ff0)
			return __builtin_nan("");
		/* x is subnormal, normalize it.  */
		ix = asuint64(x * 0x1p52);
		ix -= 52ULL << 52;
	}

	/* x = 2^k z; where z is in range [OFF,2*OFF) and exact.
	   The range is split into N subintervals.
	   The ith subinterval contains z and c is near its center.  */
	tmp = ix - OFF;
	i = (tmp >> (52 - LOG_TABLE_BITS)) % N;
	k = (int64_t)tmp >> 52; /* arithmetic shift */
	iz = ix - (tmp & 0xfffULL << 52);
	invc = T[i].invc;
	logc = T[i].logc;
	z = asdouble(iz);

	/* log(x) = log1p(z/c-1) + log(c) + k*Ln2.  */
	/* r ~= z/c - 1, |r| < 1/(2*N).  */
#if __FP_FAST_FMA
	/* rounding error: 0x1p-55/N.  */
	r = __builtin_fma(z, invc, -1.0);
#else
	/* rounding error: 0x1p-55/N + 0x1p-66.  */
	r = (z - T2[i].chi - T2[i].clo) * invc;
#endif
	kd = (double)k;

	/* hi + lo = r + log(c) + k*Ln2.  */
	w = kd * Ln2hi + logc;
	hi = w + r;
	lo = w - hi + r + kd * Ln2lo;

	/* log(x) = lo + (log1p(r) - r) + hi.  */
	r2 = r * r; /* rounding error: 0x1p-54/N^2.  */
	/* Worst case error if |y| > 0x1p-5:
	   0.5 + 4.13/N + abs-poly-error*2^57 ULP (+ 0.002 ULP without fma)
	   Worst case error if |y| > 0x1p-4:
	   0.5 + 2.06/N + abs-poly-error*2^56 ULP (+ 0.001 ULP without fma).  */
	y = lo + r2 * A[0] +
	    r * r2 * (A[1] + r * A[2] + r2 * (A[3] + r * A[4])) + hi;
	return eval_as_double(y);
}

EXPORT long double logl(long double x) {
	__ensure(!"logl is not implemented");
}

EXPORT float log2f(float x) {
	__ensure(!"log2f is not implemented");
}

EXPORT double log2(double x) {
	__ensure(!"log2 is not implemented");
}

EXPORT float log10f(float x) {
	__ensure(!"log10f is not implemented");
}

static const double
	ivln10hi  = 4.34294481878168880939e-01, /* 0x3fdbcb7b, 0x15200000 */
ivln10lo  = 2.50829467116452752298e-11, /* 0x3dbb9438, 0xca9aadd5 */
log10_2hi = 3.01029995663611771306e-01, /* 0x3FD34413, 0x509F6000 */
log10_2lo = 3.69423907715893078616e-13, /* 0x3D59FEF3, 0x11F12B36 */
Lg1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
Lg2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
Lg3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
Lg4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
Lg5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
Lg6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
Lg7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

EXPORT double log10(double x) {
	union {double f; uint64_t i;} u = {x};
	double hfsq,f,s,z,R,w,t1,t2,dk,y,hi,lo,val_hi,val_lo;
	uint32_t hx;
	int k;

	hx = u.i>>32;
	k = 0;
	if (hx < 0x00100000 || hx>>31) {
		if (u.i<<1 == 0)
			return -1/(x*x);  /* log(+-0)=-inf */
		if (hx>>31)
			return (x-x)/0.0; /* log(-#) = NaN */
		/* subnormal number, scale x up */
		k -= 54;
		x *= 0x1p54;
		u.f = x;
		hx = u.i>>32;
	} else if (hx >= 0x7ff00000) {
		return x;
	} else if (hx == 0x3ff00000 && u.i<<32 == 0)
		return 0;

	/* reduce x into [sqrt(2)/2, sqrt(2)] */
	hx += 0x3ff00000 - 0x3fe6a09e;
	k += (int)(hx>>20) - 0x3ff;
	hx = (hx&0x000fffff) + 0x3fe6a09e;
	u.i = (uint64_t)hx<<32 | (u.i&0xffffffff);
	x = u.f;

	f = x - 1.0;
	hfsq = 0.5*f*f;
	s = f/(2.0+f);
	z = s*s;
	w = z*z;
	t1 = w*(Lg2+w*(Lg4+w*Lg6));
	t2 = z*(Lg1+w*(Lg3+w*(Lg5+w*Lg7)));
	R = t2 + t1;

	/* See log2.c for details. */
	/* hi+lo = f - hfsq + s*(hfsq+R) ~ log(1+f) */
	hi = f - hfsq;
	u.f = hi;
	u.i &= (uint64_t)-1<<32;
	hi = u.f;
	lo = f - hi - hfsq + s*(hfsq+R);

	/* val_hi+val_lo ~ log10(1+f) + k*log10(2) */
	val_hi = hi*ivln10hi;
	dk = k;
	y = dk*log10_2hi;
	val_lo = dk*log10_2lo + (lo+hi)*ivln10lo + lo*ivln10hi;

	/*
	 * Extra precision in for adding y is not strictly needed
	 * since there is no very large cancellation near x = sqrt(2) or
	 * x = 1/sqrt(2), but we do it anyway since it costs little on CPUs
	 * with some parallelism and it reduces the error for many args.
	 */
	w = y + val_hi;
	val_lo += (y - w) + val_hi;
	val_hi = w;

	return val_lo + val_hi;
}

EXPORT double cbrt(double x) {
	__ensure(!"cbrt is not implemented");
}

EXPORT float expf(float x) {
	__ensure(!"expf is not implemented");
}

EXPORT double exp(double x) {
	__ensure(!"exp is not implemented");
}

EXPORT float exp2f(float x) {
	__ensure(!"exp2f is not implemented");
}

EXPORT double exp2(double x) {
	__ensure(!"exp2 is not implemented");
}

EXPORT float fmodf(float x, float y) {
	__ensure(!"fmodf is not implemented");
}

EXPORT double fmod(double x, double y) {
	__ensure(!"fmod is not implemented");
}

EXPORT long double fmodl(long double x, long double y) {
	return x - static_cast<int64_t>(x / y);
}

EXPORT float modff(float x, float* iptr) {
	__ensure(!"modff is not implemented");
}

EXPORT double modf(double x, double* iptr) {
	__ensure(!"modf is not implemented");
}

EXPORT float fminf(float x, float y) {
	__ensure(!"fminf is not implemented");
}

EXPORT double fmin(double x, double y) {
	__ensure(!"fmin is not implemented");
}

EXPORT float fmaxf(float x, float y) {
	__ensure(!"fmaxf is not implemented");
}

EXPORT double fmax(double x, double y) {
	__ensure(!"fmax is not implemented");
}

EXPORT float fmaf(float x, float y, float z) {
	__ensure(!"fmaf is not implemented");
}

EXPORT double fma(double x, double y, double z) {
	__ensure(!"fma is not implemented");
}

unsigned long long factorial(int n) {
	if (n == 0) {
		return 1;
	}
	unsigned long long result = 1;
	for (int i = 1; i <= n; ++i) {
		result *= i;
	}
	return result;
}

float powerf(float base, int exp) {
	float result = 1.0f;
	for (int i = 0; i < exp; ++i) {
		result *= base;
	}
	return result;
}

double power(double base, int exp) {
	double result = 1.0;
	for (int i = 0; i < exp; ++i) {
		result *= base;
	}
	return result;
}

EXPORT float sinf(float x) {
	//__ensure(!"sinf is not implemented");
	float result = 0.0f;
	int sign = 1;
	constexpr int TERMS = 10;

	for (int i = 0; i < TERMS; ++i) {
		int term_index = 2 * i + 1;
		result += sign * powerf(x, term_index) / factorial(term_index);
		sign = -sign;
	}
	return result;
}

EXPORT double sin(double x) {
	double result = 0.0f;
	int sign = 1;
	constexpr int TERMS = 10;

	for (int i = 0; i < TERMS; ++i) {
		int term_index = 2 * i + 1;
		result += sign * power(x, term_index) / factorial(term_index);
		sign = -sign;
	}
	return result;
}

#define M_PI 3.141592653589793238462643383279502884197

float abs(float x) {
	if (x < 0) {
		return -x;
	}
	else {
		return x;
	}
}

EXPORT float cosf(float x) {
	//__ensure(!"cosf is not implemented");
	constexpr float tp = 1./(2.*M_PI);
	x *= tp;
	x -= float(.25) + floor(x + float(.25));
	x *= float(16.) * (abs(x) - float(.5));
#if EXTRA_PRECISION
	x += T(.225) * x * (std::abs(x) - T(1.));
#endif
	return x;
}

double abs_double(double x) {
	return x < 0 ? -x : x;
}

EXPORT double cos(double x) {
	//__ensure(!"cos is not implemented");
	constexpr double tp = 1./(2.*M_PI);
	x *= tp;
	x -= double(.25) + floor(x + double(.25));
	x *= double(16.) * (abs_double(x) - double(.5));
#if EXTRA_PRECISION
	x += T(.225) * x * (std::abs(x) - T(1.));
#endif
	return x;
}

EXPORT double tan(double x) {
	__ensure(!"tan is not implemented");
}

EXPORT void sincosf(float x, float* sin, float* cos) {
	__ensure(!"sincosf is not implemented");
}

EXPORT void sincos(double x, double* sin, double* cos) {
	*sin = ::sin(x);
	*cos = ::cos(x);
}

EXPORT float hypotf(float x, float y) {
	__ensure(!"hypotf is not implemented");
}

EXPORT double sinh(double x) {
	__ensure(!"sinh is not implemented");
}

EXPORT double cosh(double x) {
	__ensure(!"cosh is not implemented");
}

EXPORT float tanhf(float x) {
	__ensure(!"tanhf is not implemented");
}

EXPORT double tanh(double x) {
	__ensure(!"tanh is not implemented");
}

EXPORT double asin(double x) {
	__ensure(!"asin is not implemented");
}

EXPORT float acosf(float x) {
	return (-0.69813170079773212f * x * x - 0.87266462599716477f) * x + 1.5707963267948966f;
}

EXPORT double acos(double x) {
	__ensure(!"acos is not implemented");
}

EXPORT float atanf(float x) {
	__ensure(!"atanf is not implemented");
}

EXPORT double atan(double x) {
	__ensure(!"atan is not implemented");
}

EXPORT float atan2f(float y, float x) {
	__ensure(!"atan2f is not implemented");
}

EXPORT double atan2(double y, double x) {
	__ensure(!"atan2 is not implemented");
}

EXPORT float floorf(float x) {
	__ensure(!"floorf is not implemented");
}

EXPORT double floor(double x) {
	if (x >= static_cast<double>(LLONG_MAX) || x <= LLONG_MIN || x != x) {
		return x;
	}
	auto n = static_cast<long long>(x);
	auto d = static_cast<double>(n);
	if (d == static_cast<double>(n) || x >= 0) {
		return d;
	}
	else {
		return d - 1;
	}
}

EXPORT float ceilf(float x) {
	//__ensure(!"ceilf is not implemented");
	return static_cast<float>(static_cast<int>(x + 1));
}

EXPORT double ceil(double x) {
	//__ensure(!"ceil is not implemented");
	return static_cast<double>(static_cast<long>(x) + 1);
}

EXPORT float roundf(float x) {
	//__ensure(!"roundf is not implemented");
	return (int) x;
}

EXPORT double round(double x) {
	__ensure(!"round is not implemented");
}

EXPORT float copysignf(float x, float y) {
	__ensure(!"copysignf is not implemented");
}

EXPORT double copysign(double x, double y) {
	__ensure(!"copysign is not implemented");
}

EXPORT double scalbn(double x, int exp) {
	union {double f; uint64_t i;} u;
	double y = x;

	if (x > 1023) {
		y *= 0x1p1023;
		x -= 1023;
		if (x > 1023) {
			y *= 0x1p1023;
			x -= 1023;
			if (x > 1023)
				x = 1023;
		}
	} else if (x < -1022) {
		/* make sure final n < -53 to avoid double
		   rounding in the subnormal range */
		y *= 0x1p-1022 * 0x1p53;
		x += 1022 - 53;
		if (x < -1022) {
			y *= 0x1p-1022 * 0x1p53;
			x += 1022 - 53;
			if (x < -1022)
				x = -1022;
		}
	}
	u.i = (uint64_t)(0x3ff+x)<<52;
	x = y * u.f;
	return x;
}

EXPORT float frexpf(float x, int* exp) {
	__ensure(!"frexpf is not implemented");
}

EXPORT double frexp(double x, int* exp) {
	union { double d; uint64_t i; } y = { x };
	int ee = y.i>>52 & 0x7ff;

	if (!ee) {
		if (x) {
			x = frexp(x*0x1p64, exp);
			*exp -= 64;
		} else *exp = 0;
		return x;
	} else if (ee == 0x7ff) {
		return x;
	}

	*exp = ee - 0x3fe;
	y.i &= 0x800fffffffffffffull;
	y.i |= 0x3fe0000000000000ull;
	return y.d;
}

EXPORT long double frexpl(long double x, int* exp) {
	__ensure(!"frexpl is not implemented");
}

EXPORT float ldexpf(float x, int exp) {
	__ensure(!"ldexpf is not implemented");
}

EXPORT double ldexp(double x, int exp) {
	return scalbn(x, exp);
}

EXPORT long lroundf(float x) {
	__ensure(!"lroundf is not implemented");
}

EXPORT long lround(double x) {
	__ensure(!"lround is not implemented");
}

EXPORT float rintf(float x) {
	__ensure(!"rintf is not implemented");
}

EXPORT double rint(double x) {
	__ensure(!"rint is not implemented");
}

EXPORT float truncf(float x) {
	__ensure(!"truncf is not implemented");
}

EXPORT double trunc(double x) {
	__ensure(!"trunc is not implemented");
}

EXPORT long lrintf(float x) {
	__ensure(!"lrintf is not implemented");
}

EXPORT long lrint(double x) {
	__ensure(!"lrint is not implemented");
}

EXPORT long long llrint(double x) {
	__ensure(!"llrint is not implemented");
}

EXPORT float nextafterf(float from, float to) {
	__ensure(!"nextafterf is not implemented");
}

EXPORT double nextafter(double from, double to) {
	if (from == to) {
		return to;
	}
	else if (from < to) {
		return hz::bit_cast<double>(hz::bit_cast<uint64_t>(from) + 1);
	}
	else {
		return hz::bit_cast<double>(hz::bit_cast<uint64_t>(from) - 1);
	}
}
