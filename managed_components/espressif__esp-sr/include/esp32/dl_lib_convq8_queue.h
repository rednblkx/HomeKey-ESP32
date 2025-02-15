// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef DL_LIB_CONVQ8_QUEUE_H
#define DL_LIB_CONVQ8_QUEUE_H


#include "dl_lib_matrixq.h"
#include "dl_lib_matrixq8.h"
#include "dl_lib_conv_queue.h"
#include "dl_lib_convq_queue.h"

#ifdef __cplusplus
extern "C" {
#endif

//[nch, n, c]
typedef struct {
    int n;           /*< the length of queue */
    int c;           /*< the number of queue element*/
    int front;       /*< the front(top) position of queue */
    int nch;         /*< the channel of queue */
    int exponent;    /*< The values in items should be multiplied by pow(2,exponent) 
                         to get the real values */
    q8tp_t *itemq;    /*< Pointer to item array */
} dl_convq8_queue_t;

/**
 * @brief Allocate a fixed-point convolution queue
 *
 * @param n     The length of queue
 * @param c     The number of elements in the queue
 * @return      The convolution queue, or NULL if out of memory
 */
dl_convq8_queue_t *dl_convq8_queue_alloc(int n, int c);

/**
 * @brief Allocate a fixed-point convolution queue
 *
 * @param n     The length of queue
 * @param c     The number of elements in the queue
 * @param c     The channel of queue
 * @return      The convolution queue, or NULL if out of memory
 */
dl_convq8_queue_t *dl_convq8_queue_alloc_mc(int n, int c, int nch);

/**
 * @brief Allocate a bit fixed-point convolution queue from PSRAM
 *
 * @param n     The length of queue
 * @param c     The number of elements in the queue
 * @param nch     The channel of queue
 * @return      The convolution queue, or NULL if out of memory
 */
dl_convq8_queue_t *dl_convq8_queue_alloc_mc_from_psram(int n, int c, int nch);

/**
 * @brief Free a fixed-point convolution queue
 *
 * @param cq     The fixed-point convolution queue to free
 */
void dl_convq8_queue_free(dl_convq8_queue_t *cq);

/**
 * @brief Set itemq of convolution queue to 0
 *
 * @param cq     The fixed-point convolution queue to free
 */
void dl_convq8_queue_bzero(dl_convq8_queue_t *cqm);

/**
 * @brief Move the front pointer of queue forward, 
          the First(oldest) element become the last(newest) element, 
 *
 * @param cq    Input fixed-point convolution queue
 * @return      Pointer of oldest element  
 */
q8tp_t *dl_convq8_queue_pop(dl_convq8_queue_t *cq);
q8tp_t *dl_convq8_queue_popn(dl_convq8_queue_t *cq, int n);

/**
 * @brief  Insert the float-point element at the end of queue.
 *         The precision of fixed-point numbers is described by the Qm.f notation,  
 *
 * @param cq     Input fixed-point convolution queue
 * @param item   The float-point element
 * @param m_bit  The number of integer bits including the sign bits
 * @param f_bit  The number of fractional bits
 */
void dl_convq8_queue_push_by_qmf(dl_convq8_queue_t *cq, fptp_t* item, int m_bit, int f_bit);

/**
 * @brief   Get the pointer of element in the queue by offset
 *
 * @param cq      Input fixed-point convolution queue
 * @param offset  Offset from the front of the queue
 * @return        Pointer of the element
 */
q8tp_t *dl_get_queue_itemq8(dl_convq8_queue_t *cq, int offset);

/**
 * @brief   Get the pointer of element in the queue by offset
 *
 * @param cq      Input fixed-point convolution queue
 * @param offset  Offset from the front of the queue
 * @param ch      Channel index of queue
 * @return        Pointer of the element
 */
q8tp_t *dl_get_queue_itemq8_mc(dl_convq8_queue_t *cq, int offset, int ch);

/**
 * @brief Fast and quantised implement for 1D atrous convolution (a.k.a. convolution with holes or dilated convolution)
 *        based on convolution queue.
 *
 * @Warning All input and output convolution queue and matrix should be allocated. The return pointer
 *          is last element of output queue and should not be freed separately.
 *
 * @param in              Input fixed-point convolution queue
 * @param out             Output fixed-point convolution queue
 * @param rate            A positive int, the stride with which we sample input value 
 * @param size            A positive int, the size of 1D-filter
 * @param kernel          Kernel matrix of filter
 * @param bias            The bias matrix of filter. Can be NULL if a bias of 0 is required.
 * @param out_exponent    Shift ratio used in dot operation between two 16-bit fixed point vector 
 * @param offset          Offset used to calculate the beginning of input conv queue  
 * @param prenum          The num to control the parameter size of preload operation  
 * @return                The result of atrous convolution
 */
void dl_atrous_conv1dq8_steps(dl_convq8_queue_t *in, dl_convq8_queue_t *out, int rate, int size,
                            dl_matrix2dq8_t* kernel, dl_matrix2dq8_t* bias, 
                            int out_exponent, int offset, int prenum);

/**
 * @brief Fast implement of dilation layer as follows
 *
 *               |-> [gate(sigmoid)] -|        
 *      input -  |                    |-> (*) - output
 *               |-> [filter(tanh)]  -|   
 *
 * @Warning All input and output convolution queue and matrix should be allocated. The return pointer
 *          is last element of output queue and should not be freed separately.
 *
 * @param in              Input fixed-point convolution queue
 * @param out             Output fixed-point convolution queue
 * @param rate            A positive int, the stride with which we sample input value 
 * @param size            A positive int, the size of 1D-filter
 * @param filter_kernel   The kernel matrix of filter
 * @param filter_bias     The bias matrix of filter. Can be NULL if a bias of 0 is required.
 * @param gate_kernel     The kernel matrix of gate
 * @param gate_bias       The bias matrix of gate. Can be NULL if a bias of 0 is required.
 * @param offset          Offset used to calculate the beginning of input conv queue  
 * @param prenum          The num to control the parameter size of preload operation  
 * @return                The result of dilation layer
 */
void dl_dilation_layerq8_steps(dl_convq8_queue_t *in, dl_convq8_queue_t *out, int rate, int size,
                                dl_matrix2dq8_t* filter_kernel, dl_matrix2dq8_t* filter_bias,
                                dl_matrix2dq8_t* gate_kernel, dl_matrix2dq8_t* gate_bias,
                                int offset, int prenum);




dl_conv_queue_t *dl_convq8_queue_add(dl_convq8_queue_t *cq1, dl_convq8_queue_t *cq2);

int8_t dl_sigmoid_lutq8(int in);
/**
 * @brief Allocate a 8-bit fixed-point Multi-Channel convolution queue
 *
 * @param n     The length of queue
 * @param c     The number of elements in the queue
 * @param nch　　The channel number
 * @return      The convolution queue, or NULL if out of memory
 */
dl_convq8_queue_t **dl_convq8_queue_mc_alloc(int n, int c, int nch);

/**
 * @brief Free a 8-bit fixed-point Multi-Channel convolution queue
 *
 * @param cqm     The fixed-point convolution queue to free
 * @param nch     The channel number
 */
void dl_convq8_queue_mc_free(dl_convq8_queue_t **cqm, int nch);

/**
 * @brief Tanh activation function for 8-bit fixed-point Multi-Channel convolution queue input
 *
 * @param cqm     Input 8-bit fixed-point Multi-Channel convolution queue
 * @param offset  Offset used to calculate the beginning of input conv queue 
 * @param nch     The channel number
 */
void dl_tanh_convq8_mc(dl_convq8_queue_t **cqm, int offset, int nch);

/**
 * @brief Fast and quantised 16-bit implement for Multi-channel 1D atrous convolution (a.k.a. convolution with holes or dilated convolution)
 *        Usually, this layer is used as first layer for 8-bit network.
 *
 * @Warning All input and output convolution queue and matrix should be allocated. The return pointer
 *          Input is a 16-bit queue point, Output is an 8-bit queue point.
 *
 * @param in              Input 16bit fixed-point convolution queue array
 * @param out             Output 8bit fixed-point convolution queue array
 * @param rate            A positive int, the stride with which we sample input value 
 * @param size            A positive int, the size of 1D-filter
 * @param kernel          The kernel matrix of filter
 * @param bias            The bias matrix of filter. Can be NULL if a bias of 0 is required.
 * @param out_exponent    Exponent of output
 * @param offset          Offset used to calculate the beginning of input conv queue  
 * @param prenum          The num to control the parameter size of preload operation            
 */
void dl_atrous_conv1dq8_16in_mc_steps(dl_convq_queue_t **in, dl_convq8_queue_t **out, int nch, int rate, int size,
                                        dl_matrix2dq_t* kernel, dl_matrix2dq_t* bias, int out_exponent, int offset, int prenum);

/**
 * @brief Fast and quantised 8-bit implement for Multi-channel 1D atrous convolution (a.k.a. convolution with holes or dilated convolution)
 *        based on convolution queue.
 *
 * @Warning All input and output convolution queue and matrix should be allocated. The return pointer
 *          is last element of output queue and should not be freed separately.
 *
 * @param in              Input 8bit fixed-point convolution queue array
 * @param out             Output 8bit fixed-point convolution queue array
 * @param rate            A positive int, the stride with which we sample input value 
 * @param size            A positive int, the size of 1D-filter
 * @param kernel          The kernel matrix of filter
 * @param bias            The bias matrix of filter. Can be NULL if a bias of 0 is required.
 * @param out_exponent    Exponent of output
 * @param offset          Offset used to calculate the beginning of input conv queue 
 * @param prenum          The num to control the parameter size of preload operation            
 */
void dl_atrous_conv1dq8_mc_steps(dl_convq8_queue_t **in, dl_convq8_queue_t **out,
                                int nch, int rate, int size,
                                dl_matrix2dq8_t* kernel, dl_matrix2dq8_t* bias, 
                                int out_exponent, int offset, int prenum);

/**
 * @brief Fast implement of 8-bit dilation layer as follows
 *
 *               |-> [gate(sigmoid)] -|        
 *      input -  |                    |-> (*) - output
 *               |-> [filter(tanh)]  -|   
 *
 * @Warning All input and output convolution queue and matrix should be allocated. The return pointer
 *          is last element of output queue and should not be freed separately.
 *
 * @param in              Input 8-bit fixed-point convolution queue
 * @param out             Output 8-bit fixed-point convolution queue
 * @param rate            A positive int, the stride with which we sample input value 
 * @param size            A positive int, the size of 1D-filter
 * @param filter_kernel   The kernel matrix of filter
 * @param filter_bias     The bias matrix of filter. Can be NULL if a bias of 0 is required.
 * @param gate_kernel     The kernel matrix of gate
 * @param gate_bias       The bias matrix of gate. Can be NULL if a bias of 0 is required.
 * @param offset          Offset used to calculate the beginning of input conv queue 
 * @param prenum          The num to control the parameter size of preload operation
 */
void dl_dilation_layerq8_mc_steps(dl_convq8_queue_t **in, dl_convq8_queue_t **out, int nch, int rate, int size,
                                    dl_matrix2dq8_t* filter_kernel, dl_matrix2dq8_t* filter_bias,
                                    dl_matrix2dq8_t* gate_kernel, dl_matrix2dq8_t* gate_bias,
                                    int offset, int prenum);    

void dl_convq8_queue_mc_bzero(dl_convq8_queue_t **cqm, int nch);



dl_convq8_queue_t *dl_convq8_queue_alloc_from_psram(int n, int c);

qtp_t *dl_dilation_layerq16_8(dl_convq_queue_t *in, dl_convq8_queue_t *out, int rate, int size,
                            dl_matrix2dq_t* filter_kernel, dl_matrix2dq_t* filter_bias,
                            dl_matrix2dq_t* gate_kernel, dl_matrix2dq_t* gate_bias, int prenum);


qtp_t *dl_dilation_layerq8(dl_convq8_queue_t *in, dl_convq8_queue_t *out, int rate, int size,
                            dl_matrix2dq8_t* filter_kernel, dl_matrix2dq_t* filter_bias,
                            dl_matrix2dq8_t* gate_kernel, dl_matrix2dq_t* gate_bias, int prenum);

dl_matrix2dq8_t *dl_convq8_lstm_layer(const dl_convq8_queue_t *in, dl_convq8_queue_t *out, dl_matrix2dq8_t *state_c,
                                      dl_matrix2dq8_t *state_h, const dl_matrix2dq8_t *in_weight, const dl_matrix2dq8_t *h_weight,
                                      const dl_matrix2dq_t *bias, int prenum);

qtp_t *dl_atrous_conv1dq8_16_s3(dl_convq8_queue_t *in, dl_convq_queue_t *out, int rate, int size,
                                 dl_matrix2dq8_t* kernel, dl_matrix2dq_t* bias, int prenum);

void print_convq8(dl_convq8_queue_t *cq, int offset);
void print_convq(dl_convq_queue_t *cq, int offset);

void lstmq8_free(void);

#ifdef __cplusplus
}
#endif

#endif
