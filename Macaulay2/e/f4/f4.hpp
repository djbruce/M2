// Copyright 2005 Michael E. Stillman

#ifndef __f4gb_h_
#define __f4gb_h_

// My implementation of Faugere's linear algebra GB routines.  Also includes free resolution code.

// Template parameters include:
//    coefficient ring arithmetic
//    packed_monomial
//    ntuple
//    varpower_monomial
// Types to define
//    MonomialLookupTable
//      a. find_divisor(packed_monomial, comp) --> integer whose lead term divides packed_monomial
//      b. insert(packed_monomial, comp, index)
//    packed_monomial
//      implemented as packed exponent vector, perhaps with weight vector(s).
//        monomial compare is done on a order by order basis (i.e. a different function)
//        each packed_monomial also includes its hash code.
//        multiplication: checked, and unchecked.
//        quotient, lcm, gcd?
//    coefficient arithmetic
//      coeff types include: ZZ/p, GF(q), ZZ, QQ, QQ[x]/f(x), other tower extensions?
//                           kk(a,...,z)
//      this is done at "low" level: we try to do as many ZZ ops as possible before reducing mod p.
//      required ops: add, mult, negate, is_zero, divide(in ZZ/p,GF), what else?
//      also required: translation to/from M2 ring_elem.
//    spair
//      spairs and gens
//      make_spair, make_gen
//      where to store the packed_monomial's?
//    SPairSet
//      insert spair
//      prepare for next degree
//      next spair
//      remove redundant pairs
//    Minimalize a set of spairs
//      Used in finding new spairs.
//    Creation of the matrix
//      This is the heart of the matter.
//      Need:
//        hashtable for column monomials (packed_monomial,comp)'s.
//        sort the list of columns
//        column header
//        row header
//        several types of rows:
//         (a) one that is essentially a list of monomials
//         (b) one after arithmetic has been performed
//         how should these rows be implemented?
//       row reduction of the matrix (using ZZ/p to get QQ).
//       rows --> new gb elements
//    gb itself
//    generators
//    
//    syzygies via this method
//    minimalization of these syzygies
//
//    keep in mind:
//      skew commutative multiplication
//      Schreyer orders
//      quotient rings
//      Hilbert function use
//      

#include "f4-types.hpp"
#include "f4-mem.hpp"
#include "monhashtable.hpp"
#include "memblock.hpp"
#include "monsort.hpp"
#include "f4-spairs.hpp"
#include "gausser.hpp"
#include "f4-m2-interface.hpp"
#include "../mat.hpp"
#include "../text-io.hpp"

/////////////////////////////////////////////////////////////////////////////

class F4GB : public our_new_delete
{
  typedef MonomialHashTable<MonomialInfo> MonomialHash;

  // Basic required information
  const Gausser *KK;
  const MonomialInfo *M;
  const FreeModule *F;  // used for debugging only...
  FreeModule *syzF; // syzygies ambient module (gebug only)
  M2_arrayint weights; // The length of this is the number of variables, each entry is positive.
  M2_arrayint component_degrees; // Degree of each free module element.
  // Also need Schreyer order info sometimes
  
  // Options and information about the computation
  bool using_syz;
  int n_lcmdups;
  int n_pairs_computed;
  int n_gens_left;
  int n_subring;
  int complete_thru_this_degree;
  int this_degree; // The current degree we are working on

  // Hilbert function information
  // Maybe this will be an external class
  
  // Monomial order information.  Should this be in M?
  
  // The main players in the computation
  gb_array gens;
  gb_array gb;
  gb_array syz_basis;
  MonomialLookupTable *lookup; // (monom,comp) --> index into gb
  F4SPairSet *S;
  
  // The matrix and its construction
  int next_col_to_process;
  coefficient_matrix *mat;
  MonomialHashTable<MonomialInfo> H;
  MemoryBlock<monomial_word> B;
  monomial_word *next_monom; // valid while creating the matrix
  
  // The syzygy matrix and its construction
  int syz_next_col_to_process;
  coefficient_matrix *syz; // recreated at every degree       
  coefficient_matrix *master_syz; // collects all syzygies
  MonomialHashTable<MonomialInfo> syzH;
  MonomialHashTable<MonomialInfo> master_syzH;
  MemoryBlock<monomial_word> syzB;
  MemoryBlock<monomial_word> master_syzB;
  monomial_word *syz_next_monom; // valid while creating <syz>
  monomial_word *master_syz_next_monom; 
  // F4Vec syzF4Vec; // syzygy vector manager 
  
  // Local data for gaussian elimination
  dense_row gauss_row, syz_row;
private:

  ////////////////////////////////////
  // Syzygy matrix manipulations:
  
  // fill syz_row from syz->row[i]
  void syz_dense_row_fill_from_sparse(int i);

  // convert dense <syz_row> into sparse <s>
  void syz_dense_row_to_sparse_row(row_elem& s);
  
  // record "reduce current row (w/leading coeff <li>) with row[pivot] (...<lj>)" 
  void syzygy_row_record_reduction(int pivot, int li, int lj); 
  
  // divide row <i> with coefficient <c>
  void syzygy_row_divide(int i, int c);

  // insert syzygy corresponding to row r 
  // ( if g>=0 then this row resulted in gb[g] )  
  void insert_syz(row_elem &r, int g = -1);

  // end (SYZYGY MANIPULATIONS)
  ////////////////////////////////////////////////////////////////////
  void delete_gb_array(gb_array &g);


  void gauss_reduce_linbox(); // dumps matrices in linbox format 

  void test_spair_code(); // test routine: probably will be removed

  enum ComputationStatusCode computation_is_complete(StopConditions &stop_);

  void do_spairs();

  void make_matrix();

    void reset_matrix();
    void clear_matrix();
    void reset_syz_matrix();
    void reset_master_syz();
    void clear_syz_matrix();
    void clear_master_syz();
    int new_column(packed_monomial m);
    int syz_new_column(packed_monomial m);
    int find_or_append_column(packed_monomial m);
    int syz_find_or_append_column(packed_monomial m);
    int mult_monomials(packed_monomial m, packed_monomial n);
    void load_gen(int which);
    void load_row(packed_monomial monom, int which);
    void syz_load_gen(int which);
    void syz_load_row(packed_monomial monom, int which);
    void process_column(int c);
    void process_s_pair(spair *p);
    void reorder_columns();
    void syz_reorder_columns();
    void reorder_rows();

    F4CoefficientArray get_coeffs_array(row_elem &r);
    // If r.coeffs is set, returns that, otherwise returns the coeffs array from
    // the generator or GB element.  The resulting value should not be modified.

    bool is_new_GB_row(int row) const;
    // returns true if the r-th row has its lead term not in the current GB
    // This can be used to determine which elements should be reduced in the first place
    // and also to determine if an element (row) needs to be tail reduced

    void gauss_reduce(bool diagonalize);
    void tail_reduce();
  
    void row_to_dense_row(int r, int &first, int &last);
    void subtract1(int r, int &first, int &last);
    void reduce1(int r, int &first, int &last);
    void dense_row_to_row(int r, int &first, int &last);

  void new_GB_elements();
  
    void insert_gb_element(row_elem &r);

public:
  F4GB(const Gausser *KK0,
       const MonomialInfo *MI,
       const FreeModule *F, // used for debugging only...
       M2_bool collect_syz, 
       int n_rows_to_keep,
       M2_arrayint gb_weights,
       int strategy, 
       M2_bool use_max_degree,
       int max_degree);
  
  ~F4GB();
  
  void set_generators(gb_array &new_gens);
  // This grabs these elements, possibly by doing a swap

  void new_generators(int lo, int hi);

  const gb_array &get_generators() const { return gens; }
  gb_array &get_generators() { return gens; }

  const gb_array &get_gb() const { return gb; }
  gb_array &get_gb() { return gb; }

  enum ComputationStatusCode start_computation(StopConditions &stop_);
  // ComputationStatusCode is defined in ../engine.h

  // Debugging routines
  void show_gb_array(const gb_array &g) const;
  void append_to_syzF(const_packed_monomial m, int compare_number);
  void show_syz_basis();  
  void show_row_info() const;
  void show_column_info() const;
  void show_matrix();
  void show_syz_matrix();
  void show_new_rows_matrix();

};

#endif

// Local Variables:
// compile-command: "make -C $M2BUILDDIR/Macaulay2/e "
// End:
