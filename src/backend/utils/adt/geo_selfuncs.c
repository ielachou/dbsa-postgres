/*-------------------------------------------------------------------------
 *
 * geo_selfuncs.c
 *	  Selectivity routines registered in the operator catalog in the
 *	  "oprrest" and "oprjoin" attributes.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/geo_selfuncs.c
 *
 *	XXX These are totally bogus.  Perhaps someone will make them do
 *	something reasonable, someday.
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"
#include <stdio.h>

#include "utils/builtins.h"
#include "utils/geo_decls.h"

#include <ctype.h>
#include <math.h>

#include "access/brin.h"
#include "access/brin_page.h"
#include "access/gin.h"
#include "access/table.h"
#include "access/tableam.h"
#include "access/visibilitymap.h"
#include "catalog/pg_am.h"
#include "catalog/pg_collation.h"
#include "catalog/pg_operator.h"
#include "catalog/pg_statistic.h"
#include "catalog/pg_statistic_ext.h"
#include "executor/nodeAgg.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "optimizer/clauses.h"
#include "optimizer/cost.h"
#include "optimizer/optimizer.h"
#include "optimizer/pathnode.h"
#include "optimizer/paths.h"
#include "optimizer/plancat.h"
#include "parser/parse_clause.h"
#include "parser/parsetree.h"
#include "statistics/statistics.h"
#include "storage/bufmgr.h"
#include "utils/acl.h"
#include "utils/date.h"
#include "utils/datum.h"
#include "utils/fmgroids.h"
#include "utils/index_selfuncs.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/pg_locale.h"
#include "utils/rel.h"
#include "utils/selfuncs.h"
#include "utils/snapmgr.h"
#include "utils/spccache.h"
#include "utils/syscache.h"
#include "utils/timestamp.h"
#include "utils/typcache.h"

#include "access/htup_details.h"
#include "catalog/pg_statistic.h"
#include "nodes/pg_list.h"
#include "optimizer/pathnode.h"
#include "optimizer/optimizer.h"
#include "utils/lsyscache.h"
#include "utils/typcache.h"
#include "utils/selfuncs.h"
#include "utils/rangetypes.h"


/*
 *	Selectivity functions for geometric operators.  These are bogus -- unless
 *	we know the actual key distribution in the index, we can't make a good
 *	prediction of the selectivity of these operators.
 *
 *	Note: the values used here may look unreasonably small.  Perhaps they
 *	are.  For now, we want to make sure that the optimizer will make use
 *	of a geometric index if one is available, so the selectivity had better
 *	be fairly small.
 *
 *	In general, GiST needs to search multiple subtrees in order to guarantee
 *	that all occurrences of the same key have been found.  Because of this,
 *	the estimated cost for scanning the index ought to be higher than the
 *	output selectivity would indicate.  gistcostestimate(), over in selfuncs.c,
 *	ought to be adjusted accordingly --- but until we can generate somewhat
 *	realistic numbers here, it hardly matters...
 */


/*
 * Selectivity for operators that depend on area, such as "overlap".
 */

Datum
areasel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(0.005);
}

Datum
areajoinsel(PG_FUNCTION_ARGS)
{
	PlannerInfo *root = (PlannerInfo *) PG_GETARG_POINTER(0);
    Oid         operator = PG_GETARG_OID(1);
    List       *args = (List *) PG_GETARG_POINTER(2);
    JoinType    jointype = (JoinType) PG_GETARG_INT16(3);
    SpecialJoinInfo *sjinfo = (SpecialJoinInfo *) PG_GETARG_POINTER(4);
    Oid         collation = PG_GET_COLLATION();

    double      selec = 0.005;

    VariableStatData vardata1;
    VariableStatData vardata2;
    Oid         opfuncoid;
    AttStatsSlot sslot1, sslot2;
    int         nhist;
    RangeBound *hist_lower1, *hist_lower2;
    RangeBound *hist_upper1, *hist_upper2;
	Datum *hist_freq1, *hist_freq2;
    int         i;
    Form_pg_statistic stats1 = NULL;
    TypeCacheEntry *typcache = NULL;
    bool        join_is_reversed;
    bool        empty;


    get_join_variables(root, args, sjinfo,
                       &vardata1, &vardata2, &join_is_reversed);

    typcache = range_get_typcache(fcinfo, vardata1.vartype);
    opfuncoid = get_opcode(operator);

    memset(&sslot1, 0, sizeof(sslot1));
	memset(&sslot2, 0, sizeof(sslot2));

/*Bound Histogram left Var */
    /* Can't use the histogram with insecure range support functions */
    if (!statistic_proc_security_check(&vardata1, opfuncoid))
        PG_RETURN_FLOAT8((float8) selec);

    if (HeapTupleIsValid(vardata1.statsTuple))
    {
        stats1 = (Form_pg_statistic) GETSTRUCT(vardata1.statsTuple);
        /* Try to get fraction of empty ranges */
        if (!get_attstatsslot(&sslot1, vardata1.statsTuple,
                             STATISTIC_KIND_BOUNDS_HISTOGRAM,
                             InvalidOid, ATTSTATSSLOT_VALUES))
        {
            ReleaseVariableStats(vardata1);
            ReleaseVariableStats(vardata2);
            PG_RETURN_FLOAT8((float8) selec);
        }
    }
    nhist = sslot1.nvalues;
    hist_lower1 = (RangeBound *) palloc(sizeof(RangeBound) * nhist);
	hist_upper1 = (RangeBound *) palloc(sizeof(RangeBound) * nhist);
	    for (i = 0; i < nhist; i++)
    {
        range_deserialize(typcache, DatumGetRangeTypeP(sslot1.values[i]),
                          &hist_lower1[i], &hist_upper1[i], &empty);
        /* The histogram should not contain any empty ranges */
        if (empty)
            elog(ERROR, "bounds histogram contains an empty range");
    }


	typcache = range_get_typcache(fcinfo, vardata2.vartype);
	/*Bound Histogram right var*/
	/* Can't use the histogram with insecure range support functions */
    if (!statistic_proc_security_check(&vardata2, opfuncoid))
        PG_RETURN_FLOAT8((float8) selec);

    if (HeapTupleIsValid(vardata2.statsTuple))
    {
        stats1 = (Form_pg_statistic) GETSTRUCT(vardata2.statsTuple);
        /* Try to get fraction of empty ranges */
        if (!get_attstatsslot(&sslot2, vardata2.statsTuple,
                             STATISTIC_KIND_BOUNDS_HISTOGRAM,
                             InvalidOid, ATTSTATSSLOT_VALUES))
        {
            ReleaseVariableStats(vardata1);
            ReleaseVariableStats(vardata2);
            PG_RETURN_FLOAT8((float8) selec);
        }
    }
    hist_upper2 = (RangeBound *) palloc(sizeof(RangeBound) * nhist);
	hist_lower2 = (RangeBound *) palloc(sizeof(RangeBound) * nhist);
    for (i = 0; i < nhist; i++)
    {
        range_deserialize(typcache, DatumGetRangeTypeP(sslot2.values[i]),
                          &hist_lower2[i], &hist_upper2[i], &empty);
        /* The histogram should not contain any empty ranges */
        if (empty)
            elog(ERROR, "bounds histogram contains an empty range");
    }

    /* Freq Histogram left var*/
    typcache = range_get_typcache(fcinfo, vardata1.vartype);
	memset(&sslot1, 0, sizeof(sslot1));
    if (HeapTupleIsValid(vardata1.statsTuple)){
        stats1 = (Form_pg_statistic) GETSTRUCT(vardata1.statsTuple);

    if (!get_attstatsslot(&sslot1, vardata1.statsTuple,
                            STATISTIC_KIND_FREQ_HISTOGRAM, 
                    OID_RANGE_OVERLAP_OP, ATTSTATSSLOT_VALUES)){
        ReleaseVariableStats(vardata1);
        ReleaseVariableStats(vardata2);
        PG_RETURN_FLOAT8((float8) selec);
    }
    printf("hihihihihi");
    printf("%d", sslot1.nvalues);
    for ( i = 0; i < 10; i++){
        printf("%d\n", DatumGetInt32(sslot1.values[i]));
    }
    printf("finihihihih");




    printf("hist_lower = [");
    for (i = 0; i < nhist; i++)
    {
        printf("%d", DatumGetInt16(hist_lower1[i].val));
        if (i < nhist - 1)
            printf(", ");
    }
    printf("]\n");
    printf("hist_upper = [");
    for (i = 0; i < nhist; i++)
    {
        printf("%d", DatumGetInt16(hist_upper1[i].val));
        if (i < nhist - 1)
            printf(", ");
    }
    printf("]\n");

        printf("hist_lower = [");
    for (i = 0; i < nhist; i++)
    {
        printf("%d", DatumGetInt16(hist_lower2[i].val));
        if (i < nhist - 1)
            printf(", ");
    }
    printf("]\n");
    printf("hist_upper = [");
    for (i = 0; i < nhist; i++)
    {
        printf("%d", DatumGetInt16(hist_upper2[i].val));
        if (i < nhist - 1)
            printf(", ");
    }
    printf("]\n");


    fflush(stdout);

    pfree(hist_lower1);
    pfree(hist_upper1);
	pfree(hist_lower2);
	pfree(hist_upper2);

    free_attstatsslot(&sslot1);
	free_attstatsslot(&sslot2);

    ReleaseVariableStats(vardata1);
    ReleaseVariableStats(vardata2);

    CLAMP_PROBABILITY(selec);
    PG_RETURN_FLOAT8((float8) selec);


	/*PlannerInfo *root = (PlannerInfo *) PG_GETARG_POINTER(0);
    Oid         operator = PG_GETARG_OID(1);
    List       *args = (List *) PG_GETARG_POINTER(2);
    JoinType    jointype = (JoinType) PG_GETARG_INT16(3);
    SpecialJoinInfo *sjinfo = (SpecialJoinInfo *) PG_GETARG_POINTER(4);
    Oid         collation = PG_GET_COLLATION();

	double      selec = 0.005;

    VariableStatData vardata1;
    VariableStatData vardata2;

	AttStatsSlot sslot1;
	int         i;
	bool join_is_reversed;
	Form_pg_statistic stats1 = NULL;
    TypeCacheEntry *typcache = NULL;
	

	get_join_variables(root, args, sjinfo,
                       &vardata1, &vardata2, &join_is_reversed);
	typcache = range_get_typcache(fcinfo, vardata1.vartype);
	memset(&sslot1, 0, sizeof(sslot1));
	    if (HeapTupleIsValid(vardata1.statsTuple)){
        stats1 = (Form_pg_statistic) GETSTRUCT(vardata1.statsTuple);

        if (!get_attstatsslot(&sslot1, vardata1.statsTuple,
                             STATISTIC_KIND_FREQ_HISTOGRAM, 
						OID_RANGE_OVERLAP_OP, ATTSTATSSLOT_VALUES)){
            ReleaseVariableStats(vardata1);
            ReleaseVariableStats(vardata2);
            PG_RETURN_FLOAT8((float8) selec);
        }
		printf("%d", sslot1.nvalues);
		for ( i = 0; i < 10; i++){
				printf("%d\n", DatumGetInt32(sslot1.values[i]));
			}
		free_attstatsslot(&sslot1);*/

		
    }




/*
 *	positionsel
 *
 * How likely is a box to be strictly left of (right of, above, below)
 * a given box?
 */

Datum
positionsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(0.1);
}

Datum
positionjoinsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(0.1);
}

/*
 *	contsel -- How likely is a box to contain (be contained by) a given box?
 *
 * This is a tighter constraint than "overlap", so produce a smaller
 * estimate than areasel does.
 */

Datum
contsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(0.001);
}

Datum
contjoinsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(0.001);
}
