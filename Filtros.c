/*
 * Filtros.c
 *
 *  Created on: 29 nov. 2020
 *      Author: urik_
 */

#include "filtros.h"

arm_fir_instance_f32 FIR[3];
static float32_t sysOutput[LENGTH_SAMPLES];
static float32_t fir_val[3][BLOCK + VAL_NUMBERS -1];

/* FILTERS IN MATLAB */
const float32_t LP_300[VAL_NUMBERS] =
{
		0.00263546835245304,0.00279637452361145,0.00321245239093617,0.00388627310232782,
		0.00481525539714661,0.00599162039210969,0.00740245630427665,0.00902989313056858,
		0.0108513848368662,0.0128400941768267,0.0149653729180457,0.0171933280561963,
		0.0194874625983500,0.0218093777424831,0.0241195218133822,0.0263779701713844,
		0.0285452195178197,0.0305829795997290,0.0324549452778385,0.0341275322683740,
		0.0355705605945668,0.0367578708722061,0.0376678599813571,0.0382839244114113,
		0.0385948015697332,0.0385948015697332,0.0382839244114113,0.0376678599813571,
		0.0367578708722061,0.0355705605945668,0.0341275322683740,0.0324549452778385,
		0.0305829795997290,0.0285452195178197,0.0263779701713844,0.0241195218133822,
		0.0218093777424831,0.0194874625983500,0.0171933280561963,0.0149653729180457,
		0.0128400941768267,0.0108513848368662,0.00902989313056858,0.00740245630427665,
		0.00599162039210969,0.00481525539714661,0.00388627310232782,0.00321245239093617,
		0.00279637452361145,0.00263546835245304
};
const float32_t HP_5000[VAL_NUMBERS] =
{
		-3.76691458426420e-18,-0.000706907267364315,-0.00133663783719985,-0.00161009047137108,
		-0.00112664805765382,0.000382668643451913,0.00268414009799595,0.00482437020638510,
		0.00533913030894429,0.00295465612300036,-0.00247622390169689,-0.00932018796561678,
		-0.0143189341367574,-0.0137706307173037,-0.00542771740201181,0.00968789009703671,
		0.0265234215347555,0.0369571592253097,0.0324476414158707,0.00751703971258194,
		-0.0373235621220030,-0.0945611281110206,-0.151269323242315,-0.192944688218000,
		0.791389729198320,-0.192944688218000,-0.151269323242315,-0.0945611281110206,
		-0.0373235621220030,0.00751703971258194,0.0324476414158707,0.0369571592253097,
		0.0265234215347555,0.00968789009703671,-0.00542771740201181,-0.0137706307173037,
		-0.0143189341367574,-0.00932018796561678,-0.00247622390169689,0.00295465612300036,
		0.00533913030894429,0.00482437020638510,0.00268414009799595,0.000382668643451913,
		-0.00112664805765382,-0.00161009047137108,-0.00133663783719985,-0.000706907267364315,
		-3.76691458426420e-18
};
const float32_t BP_300_5000[VAL_NUMBERS] =
{
		-0.000857355230901581,-0.000204843792924218,0.000284151533388043,0.000330066195956569,
		-0.000466271446902651,-0.00237069365978988,-0.00514498086675885,-0.00783013932713607,
		-0.00895517281185663,-0.00723792013001174,-0.00252055578617604,0.00357686421436148,
		0.00781154834785777,0.00649814937221832,-0.00259533659306915,-0.0184342058478405,
		-0.0359566056833580,-0.0470348930879685,-0.0431228290049159,-0.0187362137818495,
		0.0256241168708554,0.0824599158744762,0.138862634797902,0.180346156897093,0.195596419700235,
		0.180346156897093,0.138862634797902,0.0824599158744762,0.0256241168708554,-0.0187362137818495,
		-0.0431228290049159,-0.0470348930879685,-0.0359566056833580,-0.0184342058478405,
		-0.00259533659306915,0.00649814937221832,0.00781154834785777,0.00357686421436148,
		-0.00252055578617604,-0.00723792013001174,-0.00895517281185663,-0.00783013932713607,
		-0.00514498086675885,-0.00237069365978988,-0.000466271446902651,0.000330066195956569,
		0.000284151533388043,-0.000204843792924218,-0.000857355230901581
};

/*Variables to FIR */
uint32_t Block_Size = BLOCK;
uint32_t Blocks = LENGTH_SAMPLES/BLOCK;

//float32_t snr;

/*Global variables to manage buffers and filters*/
float32_t *input_32, *output_32;

void FILTER_LP(){
	arm_fir_init_f32(&FIR[LP], VAL_NUMBERS, (float32_t*)&LP_300[0], &fir_val[0][0], Block_Size);
}
void FILTER_HP(){
	arm_fir_init_f32(&FIR[HP], VAL_NUMBERS, (float32_t*)&HP_5000[0], &fir_val[1][0], Block_Size);
}
void FILTER_BP(){
	arm_fir_init_f32(&FIR[BP], VAL_NUMBERS, (float32_t*)&BP_300_5000[0], &fir_val[2][0], Block_Size);
}


void Data_Buffer(uint32_t *Buffer)
{
	input_32 = (float32_t*)Buffer;
	output_32 = sysOutput;
	FILTER_LP();
	FILTER_HP();
	FILTER_BP();
}
void Call_Filter(uint8_t Type)
{

	switch(Type){
	case 0:
		/*LOW PASS*/
		arm_fir_f32(&FIR[LP], input_32 + (4 * Block_Size), output_32 + (4 * Block_Size), Block_Size);
		break;
	case 1:
		/* HIGH PASS*/
		arm_fir_f32(&FIR[HP], input_32 + (4 * Block_Size), output_32 + (4 * Block_Size), Block_Size);
		break;
	case 2:
		/*BAND PASS*/
		arm_fir_f32(&FIR[BP], input_32 + (4 * Block_Size), output_32 + (4 * Block_Size), Block_Size);
		break;
	default:
		break;
	}
}

