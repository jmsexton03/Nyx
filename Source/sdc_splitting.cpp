
#include "Nyx.H"
#include "Nyx_F.H"

using namespace amrex;
using std::string;

void
Nyx::sdc_zeroth_step (Real time, Real dt, MultiFab& S_old, MultiFab& D_old, MultiFab& S_src)
{
    BL_PROFILE("Nyx::sdc_zeroth_step()");
    Real half_dt = 0.5*dt;

    const Real a = get_comoving_a(time);
    const Real* dx = geom.CellSize();
    int strang_comp  =   10;
#ifndef FORCING
    {
      const Real z = 1.0/a - 1.0;
      fort_interp_to_this_z(&z);
    }
#endif

#ifdef _OPENMP
#pragma omp parallel
#endif
    //    D_old.setVal(0,Sfnr_comp);
    //    D_old.setVal(-2,Ssnr_comp);
    for (MFIter mfi(S_old,true); mfi.isValid(); ++mfi)
    {
        // Note that this "bx" includes the grow cells 
        const Box& bx = mfi.growntilebox(S_old.nGrow());

        int  min_iter = 100000;
        int  max_iter =      0;
	strang_comp  =   10;

        integrate_state
                (bx.loVect(), bx.hiVect(), 
                 BL_TO_FORTRAN(S_old[mfi]),
                 BL_TO_FORTRAN(D_old[mfi]),
		 BL_TO_FORTRAN(S_src[mfi]),
                 dx, &time, &a, &dt, &min_iter, &max_iter, &strang_comp);

#ifndef NDEBUG
        if (S_old[mfi].contains_nan())
            amrex::Abort("state has NaNs after the first strang call");
#endif

    }
    //    Nyx::writeMultiFabAsPlotFile("test",
    //				     D_old,
    //				     "diag_eos")
    
}

void
Nyx::sdc_first_step (Real time, Real dt, MultiFab& S_old, MultiFab& D_old, MultiFab& S_src)
{
    BL_PROFILE("Nyx::sdc_first_step()");
    Real half_dt = 0.5*dt;

    const Real a = get_comoving_a(time);
    const Real* dx = geom.CellSize();
    int strang_comp  =   11;
#ifndef FORCING
    {
      const Real z = 1.0/a - 1.0;
      fort_interp_to_this_z(&z);
    }
#endif

#ifdef _OPENMP
#pragma omp parallel
#endif
    //    D_old.setVal(0,Sfnr_comp);
    //    D_old.setVal(-2,Ssnr_comp);
    for (MFIter mfi(S_old,true); mfi.isValid(); ++mfi)
    {
        // Note that this "bx" includes the grow cells 
        const Box& bx = mfi.growntilebox(S_old.nGrow());

        int  min_iter = 100000;
        int  max_iter =      0;
	strang_comp  =   11;

        integrate_state
                (bx.loVect(), bx.hiVect(), 
                 BL_TO_FORTRAN(S_old[mfi]),
                 BL_TO_FORTRAN(D_old[mfi]),
		 BL_TO_FORTRAN(S_src[mfi]),
                 dx, &time, &a, &dt, &min_iter, &max_iter, &strang_comp);

#ifndef NDEBUG
        if (S_old[mfi].contains_nan())
            amrex::Abort("state has NaNs after the first strang call");
#endif

    }
    //    Nyx::writeMultiFabAsPlotFile("test",
    //				     D_old,
    //				     "diag_eos")
    
}

void
Nyx::sdc_second_step (Real time, Real dt, MultiFab& S_new, MultiFab& D_new, MultiFab& S_src)
{
    BL_PROFILE("Nyx::sdc_second_step()");
    Real half_dt = 0.5*dt;
    int  min_iter = 100000;
    int  max_iter =      0;

    int min_iter_grid;
    int max_iter_grid;
    int strang_comp;

    // Set a at the half of the time step in the second strang
    const Real a = get_comoving_a(time-half_dt);
    const Real* dx = geom.CellSize();

    compute_new_temp();

#ifndef FORCING
    {
      const Real z = 1.0/a - 1.0;
      fort_interp_to_this_z(&z);
    }
#endif
    //    D_new.setVal(-1,Sfnr_comp);
    //    D_new.setVal(0,Ssnr_comp);
#ifdef _OPENMP
#pragma omp parallel private(min_iter_grid,max_iter_grid) reduction(min:min_iter) reduction(max:max_iter)
#endif
    for (MFIter mfi(S_new,true); mfi.isValid(); ++mfi)
    {
        // Here bx is just the valid region
        const Box& bx = mfi.tilebox();

        min_iter_grid = 100000;
        max_iter_grid =      0;
	strang_comp   =     12;

        integrate_state
            (bx.loVect(), bx.hiVect(), 
             BL_TO_FORTRAN(S_new[mfi]),
             BL_TO_FORTRAN(D_new[mfi]),
	     BL_TO_FORTRAN(S_src[mfi]),
             dx, &time, &a, &dt, &min_iter_grid, &max_iter_grid,&strang_comp);

        if (S_new[mfi].contains_nan(bx,0,S_new.nComp()))
        {
            std::cout << "NANS IN THIS GRID " << bx << std::endl;
        }

        min_iter = std::min(min_iter,min_iter_grid);
        max_iter = std::max(max_iter,max_iter_grid);
    }

    ParallelDescriptor::ReduceIntMax(max_iter);
    ParallelDescriptor::ReduceIntMin(min_iter);

    if (heat_cool_type == 1)
        if (ParallelDescriptor::IOProcessor())
            std::cout << "Min/Max Number of Iterations in Second Strang: " << min_iter << " " << max_iter << std::endl;
}
