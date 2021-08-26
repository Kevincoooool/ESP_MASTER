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

#ifndef _dspm_mult_H_
#define _dspm_mult_H_

#include "dsp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**@{*/
/**
 * @brief   Matrix multiplication
 *
 * Matrix multiplication for two floating point matrices: C[m][k] = A[m][n] * B[n][k]
 * The extension (_ansi) use ANSI C and could be compiled and run on any platform.
 * The extension (_ae32) is optimized for ESP32 chip.
 *
 * @param[in] A  input matrix A[m][n]
 * @param[in] B  input matrix B[n][k]
 * @param C  result matrix C[m][k]
 * @param[in] m  matrix dimension
 * @param[in] n  matrix dimension
 * @param[in] k  matrix dimension
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dspm_mult_f32_ansi(const float *A, const float *B, float *C, int m, int n, int k);
esp_err_t dspm_mult_f32_ae32(const float *A, const float *B, float *C, int m, int n, int k);
/**@}*/


/**
 * @brief   Matrix multiplication A[3x3]xB[3x1]
 *
 * Matrix multiplication for two floating point matrices 3x3 and 3x1: C[1][3] = A[3][3] * B[3][1]
 * The implementation is optimized for ESP32 chip.
 *
 * @param[in] A  input matrix A[3][3]
 * @param[in] B  input matrix/vector B[3][1]
 * @param C  result matrix/vector C[3][3]
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dspm_mult_3x3x1_f32_ae32(const float *A, const float *B, float *C);

/**
 * @brief   Matrix multiplication A[3x3]xB[3x3]
 *
 * Matrix multiplication for two square 3x3 floating point matrices: C[3][3] = A[3][3] * B[3][3]
 * The implementation is optimized for ESP32 chip.
 *
 * @param[in] A  input matrix A[3][3]
 * @param[in] B  input matrix B[3][3]
 * @param C  result matrix C[3][3]
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dspm_mult_3x3x3_f32_ae32(const float *A, const float *B, float *C);

/**
 * @brief   Matrix multiplication A[4x4]xB[4x1]
 *
 * Matrix multiplication for two floating point matrices 4x4 and 4x1: C[1][4] = A[4][4] * B[4][1]
 * The implementation is optimized for ESP32 chip.
 *
 * @param[in] A  input matrix A[4][4]
 * @param[in] B  input matrix/vector B[4][1]
 * @param C  result matrix/vector C[4][4]
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */

esp_err_t dspm_mult_4x4x1_f32_ae32(const float *A, const float *B, float *C);

/**
 * @brief   Matrix multiplication A[4x4]xB[4x4]
 *
 * Matrix multiplication for two square 3x3 floating point matrices: C[4][4] = A[4][4] * B[4][4]
 * The implementation is optimized for ESP32 chip.
 *
 * @param[in] A  input matrix A[4][4]
 * @param[in] B  input matrix B[4][4]
 * @param C  result matrix C[4][4]
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dspm_mult_4x4x4_f32_ae32(const float *A, const float *B, float *C);

/**@{*/
/**
 * @brief   Matrix multiplication 16 bit signeg int
 *
 * Matrix multiplication for two signed 16 bit fixed point matrices: C[m][k] = (A[m][n] * B[n][k]) >> (15- shift)
 * The extension (_ansi) use ANSI C and could be compiled and run on any platform.
 * The extension (_ae32) is optimized for ESP32 chip.
 *
 * @param[in] A  input matrix A[m][n]
 * @param[in] B  input matrix B[n][k]
 * @param C  result matrix C[m][k]
 * @param[in] m  matrix dimension
 * @param[in] n  matrix dimension
 * @param[in] k  matrix dimension
 * @param[in] shift every result will be shifted and stored as 16 bit signed value.
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from DSP library
 */
esp_err_t dspm_mult_s16_ansi(const int16_t *A, const int16_t *B, int16_t *C, int m, int n, int k, int shift);
esp_err_t dspm_mult_s16_ae32(const int16_t *A, const int16_t *B, int16_t *C, int m, int n, int k, int shift);
/**@}*/

#ifdef __cplusplus
}
#endif


#ifdef CONFIG_DSP_OPTIMIZED
#define dspm_mult_f32 dspm_mult_f32_ae32
#define dspm_mult_s16 dspm_mult_s16_ae32
#define dspm_mult_3x3x1_f32 dspm_mult_3x3x1_f32_ae32
#define dspm_mult_3x3x3_f32 dspm_mult_3x3x3_f32_ae32
#define dspm_mult_4x4x1_f32 dspm_mult_4x4x1_f32_ae32
#define dspm_mult_4x4x4_f32 dspm_mult_4x4x4_f32_ae32
#endif
#ifdef CONFIG_DSP_ANSI
#define dspm_mult_f32 dspm_mult_f32_ansi
#define dspm_mult_s16 dspm_mult_s16_ansi

#define dspm_mult_3x3x1_f32(A,B,C) dspm_mult_f32_ansi(A,B,C, 3, 3, 1)
#define dspm_mult_3x3x3_f32(A,B,C) dspm_mult_f32_ansi(A,B,C, 3, 3, 3)
#define dspm_mult_4x4x1_f32(A,B,C) dspm_mult_f32_ansi(A,B,C, 4, 4, 1)
#define dspm_mult_4x4x4_f32(A,B,C) dspm_mult_f32_ansi(A,B,C, 4, 4, 4)
#endif


#endif // _dspm_mult_H_