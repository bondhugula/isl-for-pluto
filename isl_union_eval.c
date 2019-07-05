/*
 * Copyright 2010      INRIA Saclay
 *
 * Use of this software is governed by the MIT license
 *
 * Written by Sven Verdoolaege, INRIA Saclay - Ile-de-France,
 * Parc Club Orsay Universite, ZAC des vignes, 4 rue Jacques Monod,
 * 91893 Orsay, France
 */

#include <isl_union_macro.h>

/* Evaluate "u" in the void point "pnt".
 * In particular, return the value NaN.
 */
static __isl_give isl_val *FN(UNION,eval_void)(__isl_take UNION *u,
	__isl_take isl_point *pnt)
{
	isl_ctx *ctx;

	ctx = isl_point_get_ctx(pnt);
	FN(UNION,free)(u);
	isl_point_free(pnt);
	return isl_val_nan(ctx);
}

/* Do the tuples of "space" correspond to those of the domain of "part"?
 * That is, is the domain space of "part" equal to "space", ignoring parameters?
 */
static isl_bool FN(PART,has_domain_space_tuples)(__isl_keep PART *part,
	__isl_keep isl_space *space)
{
	return isl_space_has_domain_tuples(space, FN(PART,peek_space)(part));
}

/* Do the tuples of "space" correspond to those of the domain of "part"?
 * That is, is the domain space of "part" equal to "space", ignoring parameters?
 */
static isl_bool FN(UNION,has_domain_space_tuples)(const void *entry,
	const void *val)
{
	PART *part = (PART *)entry;
	isl_space *space = (isl_space *) val;

	return FN(PART,has_domain_space_tuples)(part, space);
}

__isl_give isl_val *FN(UNION,eval)(__isl_take UNION *u,
	__isl_take isl_point *pnt)
{
	uint32_t hash;
	struct isl_hash_table_entry *entry;
	isl_bool is_void;
	isl_space *space;
	isl_val *v;

	if (!u || !pnt)
		goto error;
	is_void = isl_point_is_void(pnt);
	if (is_void < 0)
		goto error;
	if (is_void)
		return FN(UNION,eval_void)(u, pnt);

	space = isl_space_copy(pnt->dim);
	if (!space)
		goto error;
	hash = isl_space_get_hash(space);
	entry = isl_hash_table_find(u->space->ctx, &u->table,
				    hash, &FN(UNION,has_domain_space_tuples),
				    space, 0);
	isl_space_free(space);
	if (!entry)
		goto error;
	if (entry == isl_hash_table_entry_none) {
		v = isl_val_zero(isl_point_get_ctx(pnt));
		isl_point_free(pnt);
	} else {
		v = FN(PART,eval)(FN(PART,copy)(entry->data), pnt);
	}
	FN(UNION,free)(u);
	return v;
error:
	FN(UNION,free)(u);
	isl_point_free(pnt);
	return NULL;
}
