/***************************************************************

  Copyright (C) DSTC Pty Ltd (ACN 052 372 577) 1995.
  Unpublished work.  All Rights Reserved.

  The software contained on this media is the property of the
  DSTC Pty Ltd.  Use of this software is strictly in accordance
  with the license agreement in the accompanying LICENSE.DOC
  file.  If your distribution of this software does not contain
  a LICENSE.DOC file then you have no rights to use this
  software in any manner and should contact DSTC at the address
  below to determine an appropriate licensing arrangement.

     DSTC Pty Ltd
     Level 7, Gehrmann Labs
     University of Queensland
     St Lucia, 4072
     Australia
     Tel: +61 7 3365 4310
     Fax: +61 7 3365 4311
     Email: enquiries@dstc.edu.au

  This software is being provided "AS IS" without warranty of
  any kind.  In no event shall DSTC Pty Ltd be liable for
  damage of any kind arising out of or in connection with
  the use or performance of this software.

****************************************************************/

#ifndef KERNEL_H
#define KERNEL_H

#ifndef lint
static const char cvs_KERNEL_H[] = "$Id: kernel.h,v 1.1 1999/12/13 02:25:56 phelps Exp $";
#endif /* lint */

/* The kernel type */
typedef struct kernel *kernel_t;


/* Allocates and initializes a new kernel_t */
kernel_t kernel_alloc();

/* Releases the resources consumed by the receiver */
void kernel_free(kernel_t self);

/* Returns the Kernel's goto table */
kernel_t kernel_get_goto_table(kernel_t self);

#endif /* KERNEL_H */
