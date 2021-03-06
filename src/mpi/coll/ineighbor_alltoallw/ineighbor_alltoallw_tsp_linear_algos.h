/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

/* Header protection (i.e., INEIGHBOR_ALLTOALL_TSP_LINEAR_ALGOS_H_INCLUDED) is
 * intentionally omitted since this header might get included multiple
 * times within the same .c file. */

#include "algo_common.h"
#include "treealgo.h"
#include "tsp_namespace_def.h"

/* Routine to schedule linear algorithm for neighbor_alltoallw */
int MPIR_TSP_Ineighbor_alltoallw_sched_allcomm_linear(const void *sendbuf, const int sendcounts[],
                                                      const MPI_Aint sdispls[],
                                                      const MPI_Datatype sendtypes[], void *recvbuf,
                                                      const int recvcounts[],
                                                      const MPI_Aint rdispls[],
                                                      const MPI_Datatype recvtypes[],
                                                      MPIR_Comm * comm_ptr,
                                                      MPIR_TSP_sched_t * sched)
{
    int mpi_errno = MPI_SUCCESS;
    int indegree, outdegree, weighted;
    int k, l;
    int *srcs, *dsts;
    int tag;

    MPIR_FUNC_VERBOSE_STATE_DECL(MPID_STATE_MPIR_TSP_INEIGHBOR_ALLTOALLW_SCHED_ALLCOMM_LINEAR);
    MPIR_FUNC_VERBOSE_ENTER(MPID_STATE_MPIR_TSP_INEIGHBOR_ALLTOALLW_SCHED_ALLCOMM_LINEAR);

    MPIR_CHKLMEM_DECL(2);

    mpi_errno = MPIR_Topo_canon_nhb_count(comm_ptr, &indegree, &outdegree, &weighted);
    MPIR_ERR_CHECK(mpi_errno);
    MPIR_CHKLMEM_MALLOC(srcs, int *, indegree * sizeof(int), mpi_errno, "srcs", MPL_MEM_COMM);
    MPIR_CHKLMEM_MALLOC(dsts, int *, outdegree * sizeof(int), mpi_errno, "dsts", MPL_MEM_COMM);
    mpi_errno = MPIR_Topo_canon_nhb(comm_ptr,
                                    indegree, srcs, MPI_UNWEIGHTED,
                                    outdegree, dsts, MPI_UNWEIGHTED);
    MPIR_ERR_CHECK(mpi_errno);

    /* For correctness, transport based collectives need to get the
     * tag from the same pool as schedule based collectives */
    mpi_errno = MPIR_Sched_next_tag(comm_ptr, &tag);
    MPIR_ERR_CHECK(mpi_errno);

    for (k = 0; k < outdegree; ++k) {
        char *sb;

        sb = ((char *) sendbuf) + sdispls[k];
        MPIR_TSP_sched_isend(sb, sendcounts[k], sendtypes[k], dsts[k], tag, comm_ptr, sched, 0,
                             NULL);
    }

    for (l = 0; l < indegree; ++l) {
        char *rb;

        rb = ((char *) recvbuf) + rdispls[l];
        MPIR_TSP_sched_irecv(rb, recvcounts[l], recvtypes[l], srcs[l], tag, comm_ptr, sched, 0,
                             NULL);
    }

  fn_exit:
    MPIR_CHKLMEM_FREEALL();
    MPIR_FUNC_VERBOSE_EXIT(MPID_STATE_MPIR_TSP_INEIGHBOR_ALLTOALLW_SCHED_ALLCOMM_LINEAR);
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}


/* Non-blocking linear algo based neighbor_alltoallw */
int MPIR_TSP_Ineighbor_alltoallw_allcomm_linear(const void *sendbuf, const int sendcounts[],
                                                const MPI_Aint sdispls[],
                                                const MPI_Datatype sendtypes[], void *recvbuf,
                                                const int recvcounts[], const MPI_Aint rdispls[],
                                                const MPI_Datatype recvtypes[],
                                                MPIR_Comm * comm_ptr, MPIR_Request ** req)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_TSP_sched_t *sched;
    *req = NULL;

    MPIR_FUNC_VERBOSE_STATE_DECL(MPID_STATE_MPIR_TSP_INEIGHBOR_ALLTOALLW_ALLCOMM_LINEAR);
    MPIR_FUNC_VERBOSE_ENTER(MPID_STATE_MPIR_TSP_INEIGHBOR_ALLTOALLW_ALLCOMM_LINEAR);

    /* generate the schedule */
    sched = MPL_malloc(sizeof(MPIR_TSP_sched_t), MPL_MEM_COLL);
    MPIR_Assert(sched != NULL);
    MPIR_TSP_sched_create(sched, false);

    /* schedule linear algo */
    mpi_errno =
        MPIR_TSP_Ineighbor_alltoallw_sched_allcomm_linear(sendbuf, sendcounts, sdispls, sendtypes,
                                                          recvbuf, recvcounts, rdispls, recvtypes,
                                                          comm_ptr, sched);
    MPIR_ERR_CHECK(mpi_errno);

    /* start and register the schedule */
    mpi_errno = MPIR_TSP_sched_start(sched, comm_ptr, req);
    MPIR_ERR_CHECK(mpi_errno);

  fn_exit:
    MPIR_FUNC_VERBOSE_EXIT(MPID_STATE_MPIR_TSP_INEIGHBOR_ALLTOALLW_ALLCOMM_LINEAR);
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}
