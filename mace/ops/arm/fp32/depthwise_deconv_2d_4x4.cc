// Copyright 2019 The MACE Authors. All Rights Reserved.
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

#include <arm_neon.h>

#include "mace/ops/arm/base/depthwise_deconv_2d_4x4.h"
#include "mace/ops/arm/fp32/common_neon.h"

namespace mace {
namespace ops {
namespace arm {

template<>
MaceStatus DepthwiseDeconv2dK4x4S1<float>::DoCompute(
    const DepthwiseDeconvComputeParam &p, const float *filter_data,
    const float *input_data, float *padded_out_data) {
  p.thread_pool.Compute2D([=](index_t start0, index_t end0, index_t step0,
                              index_t start1, index_t end1, index_t step1) {
    for (index_t b = start0; b < end0; b += step0) {
      for (index_t c = start1; c < end1; c += step1) {
        const index_t offset = b * p.in_channels + c;
        float *out_base = padded_out_data + offset * p.out_img_size;
        const float *input_base = input_data + offset * p.in_img_size;
        const float *kernel_base = filter_data + c * 16;
        const float *in = input_base;
        const float *k0 = kernel_base;
        const float *k1 = kernel_base + 4;
        const float *k2 = kernel_base + 8;
        const float *k3 = kernel_base + 12;

        float32x4_t k0_vec = vld1q_f32(k0);
        float32x4_t k1_vec = vld1q_f32(k1);
        float32x4_t k2_vec = vld1q_f32(k2);
        float32x4_t k3_vec = vld1q_f32(k3);

        for (index_t i = 0; i < p.in_height; i++) {
          float *out_row = out_base + i * p.out_width;
          float *out_row_0 = out_row;
          float *out_row_1 = out_row_0 + p.out_width;
          float *out_row_2 = out_row_1 + p.out_width;
          float *out_row_3 = out_row_2 + p.out_width;
          index_t j = 0;

          for (; j + 3 < p.in_width; j += 4) {
            float32x4_t in_vec = vld1q_f32(in);

            float32x4_t out00 = vld1q_f32(out_row_0);
            out00 = neon_vfma_lane_0(out00, in_vec, k0_vec);
            vst1q_f32(out_row_0, out00);

            float32x4_t out01 = vld1q_f32(out_row_0 + 1);
            out01 = neon_vfma_lane_1(out01, in_vec, k0_vec);
            vst1q_f32(out_row_0 + 1, out01);

            float32x4_t out02 = vld1q_f32(out_row_0 + 2);
            out02 = neon_vfma_lane_2(out02, in_vec, k0_vec);
            vst1q_f32(out_row_0 + 2, out02);

            float32x4_t out03 = vld1q_f32(out_row_0 + 3);
            out03 = neon_vfma_lane_3(out03, in_vec, k0_vec);
            vst1q_f32(out_row_0 + 3, out03);

            //
            float32x4_t out10 = vld1q_f32(out_row_1);
            out10 = neon_vfma_lane_0(out10, in_vec, k1_vec);
            vst1q_f32(out_row_1, out10);

            float32x4_t out11 = vld1q_f32(out_row_1 + 1);
            out11 = neon_vfma_lane_1(out11, in_vec, k1_vec);
            vst1q_f32(out_row_1 + 1, out11);

            float32x4_t out12 = vld1q_f32(out_row_1 + 2);
            out12 = neon_vfma_lane_2(out12, in_vec, k1_vec);
            vst1q_f32(out_row_1 + 2, out12);

            float32x4_t out13 = vld1q_f32(out_row_1 + 3);
            out13 = neon_vfma_lane_3(out13, in_vec, k1_vec);
            vst1q_f32(out_row_1 + 3, out13);

            //
            float32x4_t out20 = vld1q_f32(out_row_2 + 0);
            out20 = neon_vfma_lane_0(out20, in_vec, k2_vec);
            vst1q_f32(out_row_2 + 0, out20);

            float32x4_t out21 = vld1q_f32(out_row_2 + 1);
            out21 = neon_vfma_lane_1(out21, in_vec, k2_vec);
            vst1q_f32(out_row_2 + 1, out21);

            float32x4_t out22 = vld1q_f32(out_row_2 + 2);
            out22 = neon_vfma_lane_2(out22, in_vec, k2_vec);
            vst1q_f32(out_row_2 + 2, out22);

            float32x4_t out23 = vld1q_f32(out_row_2 + 3);
            out23 = neon_vfma_lane_3(out23, in_vec, k2_vec);
            vst1q_f32(out_row_2 + 3, out23);

            //
            float32x4_t out30 = vld1q_f32(out_row_3 + 0);
            out30 = neon_vfma_lane_0(out30, in_vec, k3_vec);
            vst1q_f32(out_row_3 + 0, out30);

            float32x4_t out31 = vld1q_f32(out_row_3 + 1);
            out31 = neon_vfma_lane_1(out31, in_vec, k3_vec);
            vst1q_f32(out_row_3 + 1, out31);

            float32x4_t out32 = vld1q_f32(out_row_3 + 2);
            out32 = neon_vfma_lane_2(out32, in_vec, k3_vec);
            vst1q_f32(out_row_3 + 2, out32);

            float32x4_t out33 = vld1q_f32(out_row_3 + 3);
            out33 = neon_vfma_lane_3(out33, in_vec, k3_vec);
            vst1q_f32(out_row_3 + 3, out33);

            in += 4;
            out_row_0 += 4;
            out_row_1 += 4;
            out_row_2 += 4;
            out_row_3 += 4;
          }

          for (; j < p.in_width; j++) {
            float val = in[0];
            for (int k = 0; k < 4; ++k) {
              out_row_0[k] += val * k0[k];
              out_row_1[k] += val * k1[k];
              out_row_2[k] += val * k2[k];
              out_row_3[k] += val * k3[k];
            }
            in++;
            out_row_0++;
            out_row_1++;
            out_row_2++;
            out_row_3++;
          }
        }
      }
    }
  }, 0, p.batch, 1, 0, p.in_channels, 1);

  return MaceStatus::MACE_SUCCESS;
}

template<>
MaceStatus DepthwiseDeconv2dK4x4S2<float>::DoCompute(
    const DepthwiseDeconvComputeParam &p, const float *filter_data,
    const float *input_data, float *padded_out_data) {
  p.thread_pool.Compute2D([=](index_t start0, index_t end0, index_t step0,
                              index_t start1, index_t end1, index_t step1) {
    for (index_t b = start0; b < end0; b += step0) {
      for (index_t c = start1; c < end1; c += step1) {
        const index_t offset = b * p.in_channels + c;
        float *out_base = padded_out_data + offset * p.out_img_size;
        const float *input_base = input_data + offset * p.in_img_size;
        const float *kernel_base = filter_data + c * 16;
        const float *in = input_base;

        const float *k0 = kernel_base;
        const float *k1 = kernel_base + 4;
        const float *k2 = kernel_base + 8;
        const float *k3 = kernel_base + 12;

        float32x4_t k0_vec = vld1q_f32(k0);
        float32x4_t k1_vec = vld1q_f32(k1);
        float32x4_t k2_vec = vld1q_f32(k2);
        float32x4_t k3_vec = vld1q_f32(k3);

        for (index_t i = 0; i < p.in_height; i++) {
          float *out_row = out_base + 2 * i * p.out_width;

          float *out_row_0 = out_row;
          float *out_row_1 = out_row_0 + p.out_width;
          float *out_row_2 = out_row_1 + p.out_width;
          float *out_row_3 = out_row_2 + p.out_width;

          index_t j = 0;

          for (index_t n = 0; n + 9 < p.out_width; n += 8) {
            float32x4_t in_vec = vld1q_f32(in);

            // row 0
            float32x4x2_t out0 = vld2q_f32(out_row_0);
            out0.val[0] =
                neon_vfma_lane_0(out0.val[0], in_vec, k0_vec);
            out0.val[1] =
                neon_vfma_lane_1(out0.val[1], in_vec, k0_vec);
            vst2q_f32(out_row_0, out0);
            out0 = vld2q_f32(out_row_0 + 2);
            out0.val[0] =
                neon_vfma_lane_2(out0.val[0], in_vec, k0_vec);
            out0.val[1] =
                neon_vfma_lane_3(out0.val[1], in_vec, k0_vec);
            vst2q_f32(out_row_0 + 2, out0);

            // row 1
            float32x4x2_t out1 = vld2q_f32(out_row_1);
            out1.val[0] =
                neon_vfma_lane_0(out1.val[0], in_vec, k1_vec);
            out1.val[1] =
                neon_vfma_lane_1(out1.val[1], in_vec, k1_vec);
            vst2q_f32(out_row_1, out1);
            out1 = vld2q_f32(out_row_1 + 2);
            out1.val[0] =
                neon_vfma_lane_2(out1.val[0], in_vec, k1_vec);
            out1.val[1] =
                neon_vfma_lane_3(out1.val[1], in_vec, k1_vec);
            vst2q_f32(out_row_1 + 2, out1);

            // row 2
            float32x4x2_t out2 = vld2q_f32(out_row_2);
            out2.val[0] =
                neon_vfma_lane_0(out2.val[0], in_vec, k2_vec);
            out2.val[1] =
                neon_vfma_lane_1(out2.val[1], in_vec, k2_vec);
            vst2q_f32(out_row_2, out2);
            out2 = vld2q_f32(out_row_2 + 2);
            out2.val[0] =
                neon_vfma_lane_2(out2.val[0], in_vec, k2_vec);
            out2.val[1] =
                neon_vfma_lane_3(out2.val[1], in_vec, k2_vec);
            vst2q_f32(out_row_2 + 2, out2);

            // row 3
            float32x4x2_t out3 = vld2q_f32(out_row_3);
            out3.val[0] =
                neon_vfma_lane_0(out3.val[0], in_vec, k3_vec);
            out3.val[1] =
                neon_vfma_lane_1(out3.val[1], in_vec, k3_vec);
            vst2q_f32(out_row_3, out3);
            out3 = vld2q_f32(out_row_3 + 2);
            out3.val[0] =
                neon_vfma_lane_2(out3.val[0], in_vec, k3_vec);
            out3.val[1] =
                neon_vfma_lane_3(out3.val[1], in_vec, k3_vec);
            vst2q_f32(out_row_3 + 2, out3);

            in += 4;
            out_row_0 += 8;
            out_row_1 += 8;
            out_row_2 += 8;
            out_row_3 += 8;
            j += 4;
          }

          for (; j < p.in_width; j++) {
            float val = in[0];
            for (int k = 0; k < 4; ++k) {
              out_row_0[k] += val * k0[k];
              out_row_1[k] += val * k1[k];
              out_row_2[k] += val * k2[k];
              out_row_3[k] += val * k3[k];
            }
            in++;
            out_row_0 += 2;
            out_row_1 += 2;
            out_row_2 += 2;
            out_row_3 += 2;
          }
        }
      }
    }
  }, 0, p.batch, 1, 0, p.in_channels, 1);

  return MaceStatus::MACE_SUCCESS;
}

template<>
MaceStatus GroupDeconv2dK4x4S1<float>::DoCompute(
    const GroupDeconvComputeParam &p, const float *filter_data,
    const float *input_data, float *padded_out_data) {
  p.thread_pool.Compute3D([=](index_t start0, index_t end0, index_t step0,
                              index_t start1, index_t end1, index_t step1,
                              index_t start2, index_t end2, index_t step2) {
    for (index_t b = start0; b < end0; b += step0) {
      for (index_t g = start1; g < end1; g += step1) {
        for (index_t oc = start2; oc < end2; oc += step2) {
          if (oc + 1 < p.outch_g) {
            const index_t out_offset =
                (b * p.out_channels + p.outch_g * g + oc) * p.out_img_size;
            float *out_base = padded_out_data + out_offset;
            float *out_base1 = out_base + p.out_img_size;
            for (index_t ic = 0; ic < p.inch_g; ic++) {
              const index_t in_offset =
                  (b * p.in_channels + p.inch_g * g + ic) * p.in_img_size;
              const float *input_base = input_data + in_offset;
              const float *in = input_base;
              const index_t kernel_offset =
                  ((oc * group_ + g) * p.inch_g + ic) * 16;
              const float *kernel_base = filter_data + kernel_offset;
              const float *k0 = kernel_base;
              const float *k1 = kernel_base + 4;
              const float *k2 = kernel_base + 8;
              const float *k3 = kernel_base + 12;

              const float *kernel_base1 = kernel_base + p.in_channels * 16;
              const float *k10 = kernel_base1;
              const float *k11 = kernel_base1 + 4;
              const float *k12 = kernel_base1 + 8;
              const float *k13 = kernel_base1 + 12;

              float32x4_t k0_vec = vld1q_f32(k0);
              float32x4_t k1_vec = vld1q_f32(k1);
              float32x4_t k2_vec = vld1q_f32(k2);
              float32x4_t k3_vec = vld1q_f32(k3);

              float32x4_t k10_vec = vld1q_f32(k10);
              float32x4_t k11_vec = vld1q_f32(k11);
              float32x4_t k12_vec = vld1q_f32(k12);
              float32x4_t k13_vec = vld1q_f32(k13);

              for (index_t i = 0; i < p.in_height; i++) {
                float *out_row = out_base + i * p.out_width;

                float *out_row_0 = out_row;
                float *out_row_1 = out_row_0 + p.out_width;
                float *out_row_2 = out_row_1 + p.out_width;
                float *out_row_3 = out_row_2 + p.out_width;

                float *out_row1 = out_base1 + i * p.out_width;

                float *out_row1_0 = out_row1;
                float *out_row1_1 = out_row1_0 + p.out_width;
                float *out_row1_2 = out_row1_1 + p.out_width;
                float *out_row1_3 = out_row1_2 + p.out_width;

                index_t j = 0;

                for (; j + 3 < p.in_width; j += 4) {
                  float32x4_t in_vec = vld1q_f32(in);
                  float32x4_t out00, out01, out02, out03;
                  float32x4_t out10, out11, out12, out13;

                  out00 = vld1q_f32(out_row_0);
                  out00 = neon_vfma_lane_0(out00, in_vec, k0_vec);
                  vst1q_f32(out_row_0, out00);

                  out10 = vld1q_f32(out_row1_0);
                  out10 = neon_vfma_lane_0(out10, in_vec, k10_vec);
                  vst1q_f32(out_row1_0, out10);

                  out01 = vld1q_f32(out_row_0 + 1);
                  out01 = neon_vfma_lane_1(out01, in_vec, k0_vec);
                  vst1q_f32(out_row_0 + 1, out01);

                  out11 = vld1q_f32(out_row1_0 + 1);
                  out11 = neon_vfma_lane_1(out11, in_vec, k10_vec);
                  vst1q_f32(out_row1_0 + 1, out11);

                  out02 = vld1q_f32(out_row_0 + 2);
                  out02 = neon_vfma_lane_2(out02, in_vec, k0_vec);
                  vst1q_f32(out_row_0 + 2, out02);

                  out12 = vld1q_f32(out_row1_0 + 2);
                  out12 = neon_vfma_lane_2(out12, in_vec, k10_vec);
                  vst1q_f32(out_row1_0 + 2, out12);

                  out03 = vld1q_f32(out_row_0 + 3);
                  out03 = neon_vfma_lane_3(out03, in_vec, k0_vec);
                  vst1q_f32(out_row_0 + 3, out03);

                  out13 = vld1q_f32(out_row1_0 + 3);
                  out13 = neon_vfma_lane_3(out13, in_vec, k10_vec);
                  vst1q_f32(out_row1_0 + 3, out13);

                  //
                  out00 = vld1q_f32(out_row_1);
                  out00 = neon_vfma_lane_0(out00, in_vec, k1_vec);
                  vst1q_f32(out_row_1, out00);

                  out10 = vld1q_f32(out_row1_1);
                  out10 = neon_vfma_lane_0(out10, in_vec, k11_vec);
                  vst1q_f32(out_row1_1, out10);

                  out01 = vld1q_f32(out_row_1 + 1);
                  out01 = neon_vfma_lane_1(out01, in_vec, k1_vec);
                  vst1q_f32(out_row_1 + 1, out01);

                  out11 = vld1q_f32(out_row1_1 + 1);
                  out11 = neon_vfma_lane_1(out11, in_vec, k11_vec);
                  vst1q_f32(out_row1_1 + 1, out11);

                  out02 = vld1q_f32(out_row_1 + 2);
                  out02 = neon_vfma_lane_2(out02, in_vec, k1_vec);
                  vst1q_f32(out_row_1 + 2, out02);

                  out12 = vld1q_f32(out_row1_1 + 2);
                  out12 = neon_vfma_lane_2(out12, in_vec, k11_vec);
                  vst1q_f32(out_row1_1 + 2, out12);

                  out03 = vld1q_f32(out_row_1 + 3);
                  out03 = neon_vfma_lane_3(out03, in_vec, k1_vec);
                  vst1q_f32(out_row_1 + 3, out03);

                  out13 = vld1q_f32(out_row1_1 + 3);
                  out13 = neon_vfma_lane_3(out13, in_vec, k11_vec);
                  vst1q_f32(out_row1_1 + 3, out13);

                  //
                  out00 = vld1q_f32(out_row_2 + 0);
                  out00 = neon_vfma_lane_0(out00, in_vec, k2_vec);
                  vst1q_f32(out_row_2 + 0, out00);

                  out10 = vld1q_f32(out_row1_2 + 0);
                  out10 = neon_vfma_lane_0(out10, in_vec, k12_vec);
                  vst1q_f32(out_row1_2 + 0, out10);

                  out01 = vld1q_f32(out_row_2 + 1);
                  out01 = neon_vfma_lane_1(out01, in_vec, k2_vec);
                  vst1q_f32(out_row_2 + 1, out01);

                  out11 = vld1q_f32(out_row1_2 + 1);
                  out11 = neon_vfma_lane_1(out11, in_vec, k12_vec);
                  vst1q_f32(out_row1_2 + 1, out11);

                  out02 = vld1q_f32(out_row_2 + 2);
                  out02 = neon_vfma_lane_2(out02, in_vec, k2_vec);
                  vst1q_f32(out_row_2 + 2, out02);

                  out12 = vld1q_f32(out_row1_2 + 2);
                  out12 = neon_vfma_lane_2(out12, in_vec, k12_vec);
                  vst1q_f32(out_row1_2 + 2, out12);

                  out03 = vld1q_f32(out_row_2 + 3);
                  out03 = neon_vfma_lane_3(out03, in_vec, k2_vec);
                  vst1q_f32(out_row_2 + 3, out03);

                  out13 = vld1q_f32(out_row1_2 + 3);
                  out13 = neon_vfma_lane_3(out13, in_vec, k12_vec);
                  vst1q_f32(out_row1_2 + 3, out13);

                  //
                  out00 = vld1q_f32(out_row_3 + 0);
                  out00 = neon_vfma_lane_0(out00, in_vec, k3_vec);
                  vst1q_f32(out_row_3 + 0, out00);

                  out10 = vld1q_f32(out_row1_3 + 0);
                  out10 = neon_vfma_lane_0(out10, in_vec, k13_vec);
                  vst1q_f32(out_row1_3 + 0, out10);

                  out01 = vld1q_f32(out_row_3 + 1);
                  out01 = neon_vfma_lane_1(out01, in_vec, k3_vec);
                  vst1q_f32(out_row_3 + 1, out01);

                  out11 = vld1q_f32(out_row1_3 + 1);
                  out11 = neon_vfma_lane_1(out11, in_vec, k13_vec);
                  vst1q_f32(out_row1_3 + 1, out11);

                  out02 = vld1q_f32(out_row_3 + 2);
                  out02 = neon_vfma_lane_2(out02, in_vec, k3_vec);
                  vst1q_f32(out_row_3 + 2, out02);

                  out12 = vld1q_f32(out_row1_3 + 2);
                  out12 = neon_vfma_lane_2(out12, in_vec, k13_vec);
                  vst1q_f32(out_row1_3 + 2, out12);

                  out03 = vld1q_f32(out_row_3 + 3);
                  out03 = neon_vfma_lane_3(out03, in_vec, k3_vec);
                  vst1q_f32(out_row_3 + 3, out03);

                  out13 = vld1q_f32(out_row1_3 + 3);
                  out13 = neon_vfma_lane_3(out13, in_vec, k13_vec);
                  vst1q_f32(out_row1_3 + 3, out13);

                  in += 4;
                  out_row_0 += 4;
                  out_row_1 += 4;
                  out_row_2 += 4;
                  out_row_3 += 4;
                  out_row1_0 += 4;
                  out_row1_1 += 4;
                  out_row1_2 += 4;
                  out_row1_3 += 4;
                }

                for (; j < p.in_width; j++) {
                  float val = in[0];
                  for (int k = 0; k < 4; ++k) {
                    out_row_0[k] += val * k0[k];
                    out_row_1[k] += val * k1[k];
                    out_row_2[k] += val * k2[k];
                    out_row_3[k] += val * k3[k];
                    out_row1_0[k] += val * k10[k];
                    out_row1_1[k] += val * k11[k];
                    out_row1_2[k] += val * k12[k];
                    out_row1_3[k] += val * k13[k];
                  }
                  in++;
                  out_row_0++;
                  out_row_1++;
                  out_row_2++;
                  out_row_3++;
                  out_row1_0++;
                  out_row1_1++;
                  out_row1_2++;
                  out_row1_3++;
                }
              }
            }
          } else {
            const index_t out_offset =
                (b * p.out_channels + p.outch_g * g + oc) * p.out_img_size;
            float *out_base = padded_out_data + out_offset;
            for (index_t ic = 0; ic < p.inch_g; ++ic) {
              const index_t in_offset =
                  (b * p.in_channels + p.inch_g * g + ic) * p.in_img_size;
              const index_t kernel_offset =
                  ((oc * group_ + g) * p.inch_g + ic) * 16;

              const float *input_base = input_data + in_offset;
              const float *kernel_base = filter_data + kernel_offset;
              const float *in = input_base;
              const float *k0 = kernel_base;
              const float *k1 = kernel_base + 4;
              const float *k2 = kernel_base + 8;
              const float *k3 = kernel_base + 12;

              float32x4_t k0_vec = vld1q_f32(k0);
              float32x4_t k1_vec = vld1q_f32(k1);
              float32x4_t k2_vec = vld1q_f32(k2);
              float32x4_t k3_vec = vld1q_f32(k3);

              for (index_t i = 0; i < p.in_height; i++) {
                float *out_row = out_base + i * p.out_width;
                float *out_row_0 = out_row;
                float *out_row_1 = out_row_0 + p.out_width;
                float *out_row_2 = out_row_1 + p.out_width;
                float *out_row_3 = out_row_2 + p.out_width;
                index_t j = 0;

                for (; j + 3 < p.in_width; j += 4) {
                  float32x4_t in_vec = vld1q_f32(in);

                  float32x4_t out00 = vld1q_f32(out_row_0);
                  out00 = neon_vfma_lane_0(out00, in_vec, k0_vec);
                  vst1q_f32(out_row_0, out00);

                  float32x4_t out01 = vld1q_f32(out_row_0 + 1);
                  out01 = neon_vfma_lane_1(out01, in_vec, k0_vec);
                  vst1q_f32(out_row_0 + 1, out01);

                  float32x4_t out02 = vld1q_f32(out_row_0 + 2);
                  out02 = neon_vfma_lane_2(out02, in_vec, k0_vec);
                  vst1q_f32(out_row_0 + 2, out02);

                  float32x4_t out03 = vld1q_f32(out_row_0 + 3);
                  out03 = neon_vfma_lane_3(out03, in_vec, k0_vec);
                  vst1q_f32(out_row_0 + 3, out03);

                  //
                  float32x4_t out10 = vld1q_f32(out_row_1);
                  out10 = neon_vfma_lane_0(out10, in_vec, k1_vec);
                  vst1q_f32(out_row_1, out10);

                  float32x4_t out11 = vld1q_f32(out_row_1 + 1);
                  out11 = neon_vfma_lane_1(out11, in_vec, k1_vec);
                  vst1q_f32(out_row_1 + 1, out11);

                  float32x4_t out12 = vld1q_f32(out_row_1 + 2);
                  out12 = neon_vfma_lane_2(out12, in_vec, k1_vec);
                  vst1q_f32(out_row_1 + 2, out12);

                  float32x4_t out13 = vld1q_f32(out_row_1 + 3);
                  out13 = neon_vfma_lane_3(out13, in_vec, k1_vec);
                  vst1q_f32(out_row_1 + 3, out13);

                  //
                  float32x4_t out20 = vld1q_f32(out_row_2 + 0);
                  out20 = neon_vfma_lane_0(out20, in_vec, k2_vec);
                  vst1q_f32(out_row_2 + 0, out20);

                  float32x4_t out21 = vld1q_f32(out_row_2 + 1);
                  out21 = neon_vfma_lane_1(out21, in_vec, k2_vec);
                  vst1q_f32(out_row_2 + 1, out21);

                  float32x4_t out22 = vld1q_f32(out_row_2 + 2);
                  out22 = neon_vfma_lane_2(out22, in_vec, k2_vec);
                  vst1q_f32(out_row_2 + 2, out22);

                  float32x4_t out23 = vld1q_f32(out_row_2 + 3);
                  out23 = neon_vfma_lane_3(out23, in_vec, k2_vec);
                  vst1q_f32(out_row_2 + 3, out23);

                  //
                  float32x4_t out30 = vld1q_f32(out_row_3 + 0);
                  out30 = neon_vfma_lane_0(out30, in_vec, k3_vec);
                  vst1q_f32(out_row_3 + 0, out30);

                  float32x4_t out31 = vld1q_f32(out_row_3 + 1);
                  out31 = neon_vfma_lane_1(out31, in_vec, k3_vec);
                  vst1q_f32(out_row_3 + 1, out31);

                  float32x4_t out32 = vld1q_f32(out_row_3 + 2);
                  out32 = neon_vfma_lane_2(out32, in_vec, k3_vec);
                  vst1q_f32(out_row_3 + 2, out32);

                  float32x4_t out33 = vld1q_f32(out_row_3 + 3);
                  out33 = neon_vfma_lane_3(out33, in_vec, k3_vec);
                  vst1q_f32(out_row_3 + 3, out33);

                  in += 4;
                  out_row_0 += 4;
                  out_row_1 += 4;
                  out_row_2 += 4;
                  out_row_3 += 4;
                }

                for (; j < p.in_width; j++) {
                  float val = in[0];
                  for (int k = 0; k < 4; ++k) {
                    out_row_0[k] += val * k0[k];
                    out_row_1[k] += val * k1[k];
                    out_row_2[k] += val * k2[k];
                    out_row_3[k] += val * k3[k];
                  }
                  in++;
                  out_row_0++;
                  out_row_1++;
                  out_row_2++;
                  out_row_3++;
                }
              }
            }
          }
        }
      }
    }
  }, 0, p.batch, 1, 0, group_, 1, 0, p.outch_g, 2);

  return MaceStatus::MACE_SUCCESS;
}

template<>
MaceStatus GroupDeconv2dK4x4S2<float>::DoCompute(
    const GroupDeconvComputeParam &p, const float *filter_data,
    const float *input_data, float *padded_out_data) {
  p.thread_pool.Compute3D([=](index_t start0, index_t end0, index_t step0,
                              index_t start1, index_t end1, index_t step1,
                              index_t start2, index_t end2, index_t step2) {
    for (index_t b = start0; b < end0; b += step0) {
      for (index_t g = start1; g < end1; g += step1) {
        for (index_t oc = start2; oc < end2; oc += step2) {
          const index_t out_offset =
              (b * p.out_channels + p.outch_g * g + oc) * p.out_img_size;
          float *out_base = padded_out_data + out_offset;
          for (index_t ic = 0; ic < p.inch_g; ic++) {
            const index_t in_offset =
                (b * p.in_channels + p.inch_g * g + ic) * p.in_img_size;
            const index_t kernel_offset =
                ((oc * group_ + g) * p.inch_g + ic) * 16;
            const float *input_base = input_data + in_offset;
            const float *kernel_base = filter_data + kernel_offset;
            const float *in = input_base;

            const float *k0 = kernel_base;
            const float *k1 = kernel_base + 4;
            const float *k2 = kernel_base + 8;
            const float *k3 = kernel_base + 12;

            float32x4_t k0_vec = vld1q_f32(k0);
            float32x4_t k1_vec = vld1q_f32(k1);
            float32x4_t k2_vec = vld1q_f32(k2);
            float32x4_t k3_vec = vld1q_f32(k3);

            for (index_t i = 0; i < p.in_height; i++) {
              float *out_row = out_base + 2 * i * p.out_width;

              float *out_row_0 = out_row;
              float *out_row_1 = out_row_0 + p.out_width;
              float *out_row_2 = out_row_1 + p.out_width;
              float *out_row_3 = out_row_2 + p.out_width;

              index_t j = 0;

              for (index_t n = 0; n + 9 < p.out_width; n += 8) {
                float32x4_t in_vec = vld1q_f32(in);

                // row 0
                float32x4x2_t out0 = vld2q_f32(out_row_0);
                out0.val[0] =
                    neon_vfma_lane_0(out0.val[0], in_vec, k0_vec);
                out0.val[1] =
                    neon_vfma_lane_1(out0.val[1], in_vec, k0_vec);
                vst2q_f32(out_row_0, out0);
                out0 = vld2q_f32(out_row_0 + 2);
                out0.val[0] =
                    neon_vfma_lane_2(out0.val[0], in_vec, k0_vec);
                out0.val[1] =
                    neon_vfma_lane_3(out0.val[1], in_vec, k0_vec);
                vst2q_f32(out_row_0 + 2, out0);

                // row 1
                float32x4x2_t out1 = vld2q_f32(out_row_1);
                out1.val[0] =
                    neon_vfma_lane_0(out1.val[0], in_vec, k1_vec);
                out1.val[1] =
                    neon_vfma_lane_1(out1.val[1], in_vec, k1_vec);
                vst2q_f32(out_row_1, out1);
                out1 = vld2q_f32(out_row_1 + 2);
                out1.val[0] =
                    neon_vfma_lane_2(out1.val[0], in_vec, k1_vec);
                out1.val[1] =
                    neon_vfma_lane_3(out1.val[1], in_vec, k1_vec);
                vst2q_f32(out_row_1 + 2, out1);

                // row 2
                float32x4x2_t out2 = vld2q_f32(out_row_2);
                out2.val[0] =
                    neon_vfma_lane_0(out2.val[0], in_vec, k2_vec);
                out2.val[1] =
                    neon_vfma_lane_1(out2.val[1], in_vec, k2_vec);
                vst2q_f32(out_row_2, out2);
                out2 = vld2q_f32(out_row_2 + 2);
                out2.val[0] =
                    neon_vfma_lane_2(out2.val[0], in_vec, k2_vec);
                out2.val[1] =
                    neon_vfma_lane_3(out2.val[1], in_vec, k2_vec);
                vst2q_f32(out_row_2 + 2, out2);

                // row 3
                float32x4x2_t out3 = vld2q_f32(out_row_3);
                out3.val[0] =
                    neon_vfma_lane_0(out3.val[0], in_vec, k3_vec);
                out3.val[1] =
                    neon_vfma_lane_1(out3.val[1], in_vec, k3_vec);
                vst2q_f32(out_row_3, out3);
                out3 = vld2q_f32(out_row_3 + 2);
                out3.val[0] =
                    neon_vfma_lane_2(out3.val[0], in_vec, k3_vec);
                out3.val[1] =
                    neon_vfma_lane_3(out3.val[1], in_vec, k3_vec);
                vst2q_f32(out_row_3 + 2, out3);

                in += 4;
                out_row_0 += 8;
                out_row_1 += 8;
                out_row_2 += 8;
                out_row_3 += 8;
                j += 4;
              }

              for (; j < p.in_width; j++) {
                float val = in[0];
                for (int k = 0; k < 4; ++k) {
                  out_row_0[k] += val * k0[k];
                  out_row_1[k] += val * k1[k];
                  out_row_2[k] += val * k2[k];
                  out_row_3[k] += val * k3[k];
                }
                in++;
                out_row_0 += 2;
                out_row_1 += 2;
                out_row_2 += 2;
                out_row_3 += 2;
              }
            }
          }
        }
      }
    }
  }, 0, p.batch, 1, 0, group_, 1, 0, p.outch_g, 1);

  return MaceStatus::MACE_SUCCESS;
}

}  // namespace arm
}  // namespace ops
}  // namespace mace

