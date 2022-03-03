// Copyright 2018 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
// Author: Michael Schaffner <schaffner@iis.ee.ethz.ch>, ETH Zurich
// Date: 26.11.2018
// Description: Simple hello world program that prints the core id.
// Also runs correctly on manycore configs.
//

#include <stdint.h>
#include <stdio.h>
#include "util.h"

int main(int argc, char** argv) {
  // synchronization variable
  volatile static uint32_t amo_cnt = 0;
  // synchronize with other cores and wait until it is this core's turn
  while(argv[0][0] != amo_cnt);
  int assembly_output = 5;
  /* assembly aiming to verify that writing and reading to a USER CSR works as
  expected */
  printf("Output coming from inline assembly = %d\n", assembly_output);
  asm("and x5, x5, 0\n\t" // set the value of x5 to zero
  "addi x5, x5, 0\n\t" // set the value of x5 to two
  "csrw 0x808, x5\n\t" // write the value of x5 to the USER CSR 0x800
  "csrr x5, 0x808\n\t" // read the value that the USER CSR has in order to validate that csrw worked as expected
  "move %0, x5\n\t"
  :"=r" (assembly_output) // output
  );
  /* print out the value of the assembly_output variable in order to validate
  that the inline_assembly code worked as expected and modified the contents of the USER CSR */
  printf("Output coming from inline assembly = %d\n", assembly_output);
  // assemble number and print
  printf("Hello world, this is hart %d of %d harts!\n", argv[0][0], argv[0][1]);
  // increment atomic counter
  ATOMIC_OP(amo_cnt, 1, add, w);
return 0;
}