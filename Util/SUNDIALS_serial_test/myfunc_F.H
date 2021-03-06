#ifndef MYFUNC_F_H_
#define MYFUNC_F_H_

#ifdef __cplusplus
extern "C"
{
#endif


  void FSetIC(amrex_real* phi,
                     const int* lo,
                     const int* hi,
	             double* dptr);

  void FSetIC_mfab(amrex_real* phi,
                     const int* lo,
		   const int* hi);

  void FSetInternalEnergy_mfab(amrex_real* e,
                     const int* lo,
		   const int* hi);

  void FSetSol(amrex_real* mf,
                     const int* lo,
                     const int* hi,
	             double* dptr);

  /*  void RhsFn(realtype t, N_Vector u, N_Vector udot, void *user_data);*/
  ///  void RhsFn(double t, N_Vector u, N_Vector udot, double* rpar);
  void RhsFnReal(double t, double* u, double* udot, double* rpar, int neq);

  void fort_init_allocations();

  void fort_init_tables_eos_params(amrex_real& a);

  void fort_fin_allocations();

    void fort_fab_copytoreal(const int* lo, const int* hi,
                                  const int* bx_lo, const int* bx_hi,
                                  amrex_real* dst,
                                  const amrex_real* src, const int* slo, const int* shi,
                                  const int ncomp);

    void fort_fab_copyfromreal(const int* lo, const int* hi,
                                    const int* bx_lo, const int* bx_hi,
                                    const amrex_real* dst, const int* dlo, const int* dhi, const int ncomp,
                                    const amrex_real* src);

  /*  void FSetIC_mfab(amrex_real* mf,
                     const int* lo,
		   const int* hi);//, const int* ncomp, const int* src_comp);
  */

#ifdef __cplusplus
}
#endif

#endif
