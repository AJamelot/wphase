/***************************************************************************
*
*	              W phase source inversion package 	            
*                               -------------
*
*        Main authors: Zacharie Duputel, Luis Rivera and Hiroo Kanamori
*                      
* (c) California Institute of Technology and Universite de Strasbourg / CNRS 
*                                  April 2013
*
*    Neither the name of the California Institute of Technology (Caltech) 
*    nor the names of its contributors may be used to endorse or promote 
*    products derived from this software without specific prior written 
*    permission
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/
#define VERSION 252

#ifndef POW
#define POW 1.e28
#endif

#ifndef DEG2RAD
#define DEG2RAD (M_PI/180.)
#endif

#ifndef RX
#define	RX 17
#endif

#ifndef RY
#define	RY 9
#endif

#ifndef NM
#define NM 6
#endif 

#ifndef VAREPS
#define VAREPS 0.0000001
#endif 

#ifndef FILLCOOR
#define FILLCOOR(coor,lat,lon,dep) coor[0]=lat; coor[1]=lon; coor[2]=dep;
#define FILLCOOR2(coor,lat,lon,dep,rms) coor[0]=lat; coor[1]=lon; coor[2]=dep; coor[3]=rms;
#endif /* not FILLCOOR */

#ifndef NDEPTHS_GF
#define NDEPTHS_GF 200
#endif /* not NDEPTHS */

#ifdef _OPENMP
#include <omp.h>
#endif

#ifndef TWPTT
#define TWPTT 1
#endif

#ifndef NCOM
#define NCOM 50
#endif


typedef struct
{
  int    ts_Nit, ps, xy_Nx, xy_Nopt, xy_Nit, ncom ;
  int    hdsafe, dc_flag, ref_flag,ip,ib[4] ;
  double th_val, cth_val, df_val, med_val,ts;
  double op_pa, p2p_med, p2p_low, p2p_high  ;
  double dts_min,dts_step,dts_max, dts_val  ;
  double *rms_in,rms_r_th,*rms_r,*p2p,*avg  ; 
  double xy_dx, dlat, dlon, dz, mindep, azp ;
  double ntr_val, wZ, wN, wE, priorsdrM0[4] ;
  double *wgt, wgtnorm, dmin, dmax          ;
  char   i_master[FSIZE], i_saclst[FSIZE]   ;
  char   o_saclst[FSIZE], log[FSIZE]        ; 
  char   o_cmtf[FSIZE], p_data[FSIZE]       ;
  char   osacdir[FSIZE], psfile[FSIZE]      ; 
  char   wpbmfile[FSIZE], refbmfile[FSIZE], comments[NCOM][LSIZE];
  char   tsgsfile[FSIZE], xygsfile[FSIZE], o_covf[FSIZE] ; 
  char   inifile[FSIZE], gs_flag[2]         ;
} struct_opt ;

/* Screening routines */
void screen_med(int *nsac, char **data_name, double **data, 
		double ***G, sachdr *hd_synt, struct_opt *opt, FILE *o_log) ;
void screen_rms(int *nsac, char **data_name, double **data, 
		double ***G, sachdr *hd_synt, struct_opt *opt, FILE *o_log) ;
void screen_ratio(int *nsac,char **data_name,double **data,
		  double ***G,sachdr *hd_synt, struct_opt *opt, FILE *o_log);

/* Stats routines */
void median(int nsac, struct_opt *opt) ;
void sort(double *tab, int ns) ;
void calc_stat(int npts, double *data, double *p2p, double *ave) ;
void calc_rms_sub(int npts, double *data, double **dcalc, double *rms, int flag);
void calc_rms(int ns, sachdr *hd_synt, double **data, double ***dcalc, 
	       double **rms, double *global_rms, struct_opt *opt) ;
void calc_data_norm(double **data, sachdr *hd_synt, int nsac, double *data_norm);
void azpond(sachdr *hd_synt, int ns, double *wgt) ;

/* Inversion routines and linear algebra */
void jacobi(double **a,int n, int np, double *d, double **v, int *nrot) ;
void eigsrt(double *d, double **v, int n) ;
void free_G(double ***G) ;
int  fill_G(char *gf_file, char *datafile, sachdr *hd_GF, sachdr *hd_data, int npts, 
	    double Ptt, double twp_beg, double twp_end, double *buffer, double *G, 
	    struct_opt *opt, FILE *o_log) ;
void comp_GtG(int M, int nsac, sachdr *hd_synt, double ***G, double **GtG, 
	      struct_opt *opt) ;
void inversion(int M, sachdr *hd_synt, double ***G, double **d, 
	       struct_opt *opt, FILE *o_log, struct_quake_params *eq) ;
void lsqenp_(int *nf, int *mp, int *mv, float *yl, float *x, float *b, int *ip, 
	     int *ib, int *idvt, int *icon, int *iquit, int *iprnt) ;
void sdr2mt(double *vm,double Mo,double strike,double dip,double rake) ;
void inversion_dc(sachdr *hd_synt, double ***G, double **d,
                  double *sdrM0,struct_opt *opt, FILE *o_log, struct_quake_params *eq);
void set_wgt(int ns, sachdr *hd_data,struct_opt *opt) ;
void set_matrices (char ***sacfiles, sachdr **hd_synt, 
		   double ***data, double ****G, struct_opt *opt, struct_quake_params *eq, FILE *o_log) ;
void calc_data(int nsac, sachdr *hd_synt, double ***G, double **vm, 
	       double **data, double ***d, struct_opt *opt, FILE *o_log);
void residual_moment(double **vm, double *ma, double *mb, double *mc) ;
void mt2sm(double *vm,double *sm) ;
void get_planes(double *vm, double *eval3, double **evec3, double *strike1, double *dip1, double *rake1, double *strike2,double *dip2,double *rake2) ;
void w_log_header(char **argv, struct_opt *opt, struct_quake_params *eq, double *wp_win4, 
		  FILE *o_log) ;
void write_cmtf(char *filename, struct_quake_params *eq, double *vm) ;
void w_o_saclst(int ns, char **sacfiles, sachdr *hd_synt, double **rms, 
		double *data_norm, struct_opt *opt) ;
void get_gap(sachdr *hd_synt, int ns, double *gap) ;
int charplot(double *M, double strike1, double dip1, double strike2, double dip2, 
	     char D, char P, char W, char B, char sep, char pnod, 
	     int rx, int ry, FILE *stream) ;
void output_products(struct_opt *opt, struct_quake_params *eq,
		     sachdr *hd_synt, FILE *o_log, 
                     char **argv) ;
void prad_pat(double *vm, FILE *ps)               ;
void pnod_pat(double *strike, double *dip, FILE *ps)    ;


void set_data_vector(int nd,double *dv,double *tv,int *nsac,double ***data,char ***sacfiles,
		     sachdr **hd_synt,struct_quake_params *eq,struct_opt *opt,FILE *o_log);

void fast_synth_sub(double az, double baz, double xdeg, double *tv, double *dv, int nd, 
		    struct_quake_params *eq, sachdr *hdr, double **GFs, double *Z, double *TH, double *PH);

void fast_synth_only_Z_sub(double az, double baz, double xdeg, double *tv, double *dv, int nd, 
                           struct_quake_params *eq, sachdr *hdr, double **GFs, double *Z);

void fast_synth_only_Hs_sub(double az, double baz, double xdeg, double *tv, double *dv, int nd, 
                            struct_quake_params *eq, sachdr *hdr, double **GFs, double *TH, double *PH);

void distaz(double cmt_lat, double cmt_lon, float* stlats, float* stlons, 
	    int nstat, float* dists, float* azs, float* bazs, float* xdegs,
	    long int* nerr);  

void get_depths(char *path, double *depths, int *nd);

void rotate_traces(double *T, double *P, float baz, int npts, double *S);

int fill_kernel_G(sachdr *hd_GF,sachdr *hd_data,double Ptt,double twp_beg, 
		  double twp_end,double *elem_disp,double *G,struct_opt *opt,FILE *o_log);

void calc_kernel(struct_quake_params *eq,struct_opt *opt,sachdr *hd_synth,char *itype,
		 int nd,double *dv,double *tv, double ***G,FILE *o_log);

void copy_eq(struct_quake_params *i_eq, struct_quake_params *o_eq);

void copy_opt(struct_opt *i_opt, struct_opt *o_opt,int nsac);

void realloc_gridsearch(int nsac, double **rms, double *global_rms, 
			double ***dcalc,int flag);

void fast_ts_gridsearch(int M,int nd,double *dv,double *tv,sachdr *hd_synt,double **data, 
			double ***G,double ***dcalc,double **rms,struct_opt *opt,
			struct_quake_params *eq,double *tsopt,double *rmsopt, FILE *o_log);

void ts_gridsearch(int nsac,int M,int nd,double *dv,double *tv,sachdr *hd_synt,double **data, 
		   double *rmsini,struct_opt *opt,struct_quake_params *eq,double *tsopt,
		   double *rmsopt, FILE *o_log);

void xy_gridsearch(int M,int nd,double *dv,double *tv, sachdr *hd_synt,
		   double **data,double ***G,double ***dcalc,double **rms,
		   struct_opt *opt,struct_quake_params *eq,double *rmsopt, double *latopt,
		   double *lonopt,double *depopt,FILE *o_log);
