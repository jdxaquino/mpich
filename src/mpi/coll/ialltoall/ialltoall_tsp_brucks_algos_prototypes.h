/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

/* Header protection (i.e., IALLTOALL_TSP_BRUCKS_ALGOS_PROTOTYPES_H_INCLUDED) is
 * intentionally omitted since this header might get included multiple
 * times within the same .c file. */

#include "tsp_namespace_def.h"

#undef MPIR_TSP_Ialltoall_intra_brucks
#define MPIR_TSP_Ialltoall_intra_brucks                 MPIR_TSP_NAMESPACE(Ialltoall_intra_brucks)
#undef MPIR_TSP_Ialltoall_sched_intra_brucks
#define MPIR_TSP_Ialltoall_sched_intra_brucks           MPIR_TSP_NAMESPACE(Ialltoall_sched_intra_brucks)

int MPIR_TSP_Ialltoall_sched_intra_brucks(const void *sendbuf, MPI_Aint sendcount,
                                          MPI_Datatype sendtype, void *recvbuf, MPI_Aint recvcount,
                                          MPI_Datatype recvtype, MPIR_Comm * comm,
                                          MPIR_TSP_sched_t * s, int k, int buffer_per_phase);
int MPIR_TSP_Ialltoall_intra_brucks(const void *sendbuf, MPI_Aint sendcount, MPI_Datatype sendtype,
                                    void *recvbuf, MPI_Aint recvcount, MPI_Datatype recvtype,
                                    MPIR_Comm * comm_ptr, MPIR_Request ** request, int k,
                                    int buffer_per_phase);
