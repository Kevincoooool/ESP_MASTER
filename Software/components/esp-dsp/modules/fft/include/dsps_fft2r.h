// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _dsps_fft2r_H_
#define _dsps_fft2r_H_
#include "dsp_err.h"
#include "sdkconfig.h"

#ifndef CONFIG_DSP_MAX_FFT_SIZE
#define CONFIG_DSP_MAX_FFT_SIZE 4096
#endif // CONFIG_DSP_MAX_FFT_SIZE

#ifdef __cplusplus
extern "C"
{
#endif

extern float* dsps_fft_w_table_fc32;
extern int dsps_fft_w_table_size;
extern uint8_t dsps_fft2r_initialized;

extern int16_t* dsps_fft_w_table_sc16;
extern int dsps_fft_w_table_sc16_size;
extern uint8_t dsps_fft2r_sc16_initialized;


/**@{*/
/**
 * @brief      init fft tables
 * 
 * Initialization of Complex FFT. This function initialize coefficients table.
 * The implementation use ANSI C and could be compiled and run on any platform
 * 
 * @param[inout] fft_table_buff: pointer to floating point buffer where sin/cos table will be stored
 *                          if this parameter set to NULL, and table_size value is more then 0, then 
 *                          dsps_fft2r_init_fc32 will allocate buffer internally
 * @param[in] table_size: size of the buffer in float words  
 *                      if fft_table_buff is NULL and table_size is not 0, buffer will be allocated internally.
 *                      If table_size is 0, buffer will not be allocated. 
 * 
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_DSP_PARAM_OUTOFRANGE if table_size > CONFIG_DSP_MAX_FFT_SIZE
 *      - ESP_ERR_DSP_REINITIALIZED if buffer already allocated internally by other function
 *      - One of the error codes from DSP library
 */
esp_err_t dsps_fft2r_init_fc32(float* fft_table_buff, int table_size);
esp_err_t dsps_fft2r_init_sc16(int16_t* fft_table_buff, int table_size);
/**@}*/ 

/**@{*/
/**
 * @brief      deinit fft tables
 * 
 * Free resources of Complex FFT. This function delete coefficients table if it was allocated by dsps_fft2r_init_fc32.
 * The implementation use ANSI C and could be compiled and run on any platform
 * 
 * 
 * @return
 */
void dsps_fft2r_deinit_fc32();
void dsps_fft2r_deinit_sc16();
/**@}*/ 

/**@{*/
/**
 * @brief      complex FFT of radix 2
 * 
 * Complex FFT of radix 2
 * The extension (_ansi) use ANSI C and could be compiled and run on any platform. 
 * The extension (_ae32) is optimized for ESP32 chip.
 *
 * @param[inout] data: input/output complex array. An elements located: Re[0], Im[0], ... Re[N-1], Im[N-1]
 *               result of FFT will be stored to this array.
 * @param[in] N: Number of complex elements in input array
 *
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dsps_fft2r_fc32_ansi(float *data, int N);
esp_err_t dsps_fft2r_fc32_ae32_(float *data, int N, float* w);
esp_err_t dsps_fft2r_sc16_ansi(int16_t *data, int N);
esp_err_t dsps_fft2r_sc16_ae32_(int16_t *data, int N, int16_t* w);
/**@}*/ 
// This is workaround because linker generates permanent error when assembler uses 
// direct access to the table pointer
#define dsps_fft2r_fc32_ae32(data, N) dsps_fft2r_fc32_ae32_(data, N, dsps_fft_w_table_fc32)
#define dsps_fft2r_sc16_ae32(data, N) dsps_fft2r_sc16_ae32_(data, N, dsps_fft_w_table_sc16)


/**@{*/
/**
 * @brief      bit reverse operation for the complex input array
 * 
 * Bit reverse operation for the complex input array
 * The implementation use ANSI C and could be compiled and run on any platform
 *
 * @param[inout] data: input/ complex array. An elements located: Re[0], Im[0], ... Re[N-1], Im[N-1]
 *               result of FFT will be stored to this array.
 * @param[in] N: Number of complex elements in input array
 *
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dsps_bit_rev_fc32_ansi(float *data, int N);
esp_err_t dsps_bit_rev_sc16_ansi(int16_t *data, int N);
/**@}*/ 

/**@{*/
/**
 * @brief      Generate coefficients table for the FFT radix 2
 * 
 * Generate coefficients table for the FFT radix 2. This function called inside init.
 * The implementation use ANSI C and could be compiled and run on any platform
 *
 * @param[inout] w: memory location to store coefficients.
 *           By default coefficients will be stored to the dsps_fft_w_table_fc32.
 *           Maximum size of the FFT must be setup in menuconfig
 * @param[in] N: maximum size of the FFT that will be used
 *
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dsps_gen_w_r2_fc32(float *w, int N);
esp_err_t dsps_gen_w_r2_sc16(int16_t *w, int N);
/**@}*/ 

/**@{*/
/**
 * @brief      Convert complex array to two real arrays
 * 
 * Convert complex array to two real arrays in case if input was two real arrays.
 * This function have to be used if FFT used to process real data.
 * The implementation use ANSI C and could be compiled and run on any platform
 *
 * @param[inout] data: Input complex array and result of FFT2R.
 *               input has size of 2*N, because contains real and imaginary part.
 *               result will be stored to the same array.
 *               Input1: input[0..N-1], Input2: input[N..2*N-1]
 * @param[in] N: Number of complex elements in input array
 *
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dsps_cplx2reC_fc32_ansi(float *data, int N);
esp_err_t dsps_cplx2reC_sc16(int16_t *data, int N);
/**@}*/ 

#ifdef __cplusplus
}
#endif

#ifdef CONFIG_DSP_OPTIMIZED
#define dsps_fft2r_fc32 dsps_fft2r_fc32_ae32
#define dsps_bit_rev_fc32 dsps_bit_rev_fc32_ansi
#define dsps_cplx2reC_fc32 dsps_cplx2reC_fc32_ansi
#define dsps_fft2r_sc16 dsps_fft2r_sc16_ae32
#endif
#ifdef CONFIG_DSP_ANSI
#define dsps_fft2r_fc32 dsps_fft2r_fc32_ansi
#define dsps_bit_rev_fc32 dsps_bit_rev_fc32_ansi
#define dsps_cplx2reC_fc32 dsps_cplx2reC_fc32_ansi
#define dsps_fft2r_sc16 dsps_fft2r_sc16_ansi
#endif


#endif // _dsps_fft2r_cf_H_