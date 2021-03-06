// @HEADER
// ************************************************************************
//
//               Rapid Optimization Library (ROL) Package
//                 Copyright (2014) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact lead developers:
//              Drew Kouri   (dpkouri@sandia.gov) and
//              Denis Ridzal (dridzal@sandia.gov)
//
// ************************************************************************
// @HEADER

#ifndef ROL_LINESEARCHSTEP_H
#define ROL_LINESEARCHSTEP_H

#include "ROL_Types.hpp"
#include "ROL_HelperFunctions.hpp"
#include "ROL_Step.hpp"
#include "ROL_LineSearch.hpp"

// Unconstrained Methods
#include "ROL_GradientStep.hpp"
#include "ROL_NonlinearCGStep.hpp"
#include "ROL_SecantStep.hpp"
#include "ROL_NewtonStep.hpp"
#include "ROL_NewtonKrylovStep.hpp"

// Bound Constrained Methods
#include "ROL_ProjectedSecantStep.hpp"
#include "ROL_ProjectedNewtonStep.hpp"
#include "ROL_ProjectedNewtonKrylovStep.hpp"

#include <sstream>
#include <iomanip>

/** @ingroup step_group
    \class ROL::LineSearchStep
    \brief Provides the interface to compute optimization steps
           with line search.

    Suppose \f$\mathcal{X}\f$ is a Hilbert space of 
    functions mapping \f$\Xi\f$ to \f$\mathbb{R}\f$.  For example, 
    \f$\Xi\subset\mathbb{R}^n\f$ and \f$\mathcal{X}=L^2(\Xi)\f$ or 
    \f$\Xi = \{1,\ldots,n\}\f$ and \f$\mathcal{X}=\mathbb{R}^n\f$. We 
    assume \f$f:\mathcal{X}\to\mathbb{R}\f$ is twice-continuously Fr&eacute;chet 
    differentiable and \f$a,\,b\in\mathcal{X}\f$ with \f$a\le b\f$ almost 
    everywhere in \f$\Xi\f$.  Note that these line-search algorithms will also work 
    with secant approximations of the Hessian. 
    This step applies to unconstrained and bound constrained optimization problems,
    \f[
        \min_x\quad f(x) \qquad\text{and}\qquad \min_x\quad f(x)\quad\text{s.t.}\quad a\le x\le b,
    \f]
    respectively.  

    For unconstrained problems, given the \f$k\f$-th iterate \f$x_k\f$ and a descent direction
    \f$s_k\f$, the line search approximately minimizes the 1D objective function 
    \f$\phi_k(t) = f(x_k + t s_k)\f$.  The approximate minimizer \f$t\f$ must satisfy 
    sufficient decrease and curvature conditions into guarantee global convergence.  The 
    sufficient decrease condition (often called the Armijo condition) is 
    \f[
       \phi_k(t) \le \phi_k(0) + c_1 t \phi_k'(0) \qquad\iff\qquad
       f(x_k+ts_k) \le f(x_k) + c_1 t \langle \nabla f(x_k),s_k\rangle_{\mathcal{X}}
    \f]
    where \f$0 < c_1 < 1\f$.  The curvature conditions implemented in ROL include:

    <CENTER>
    | Name              | Condition                                                     | Parameters |
    | :---------------- | :-----------------------------------------------------------: | :---------------------: |
    | Wolfe             | \f$\phi_k'(t) \ge c_2\phi_k'(0)\f$                            | \f$c_1<c_2<1\f$         |
    | Strong Wolfe      | \f$\left|\phi_k'(t)\right| \le c_2 \left|\phi_k'(0)\right|\f$ | \f$c_1<c_2<1\f$         |
    | Generalized Wolfe | \f$c_2\phi_k'(0)\le \phi_k'(t)\le-c_3\phi_k'(0)\f$            | \f$0<c_3<1\f$           |
    | Approximate Wolfe | \f$c_2\phi_k'(0)\le \phi_k'(t)\le(2c_1-1)\phi_k'(0)\f$        | \f$c_1<c_2<1\f$         |
    | Goldstein         | \f$\phi_k(0)+(1-c_1)t\phi_k'(0)\le \phi_k(t)\f$               | \f$0<c_1<\frac{1}{2}\f$ |
    </CENTER>
    
    Note that \f$\phi_k'(t) = \langle \nabla f(x_k+ts_k),s_k\rangle_{\mathcal{X}}\f$.

    For bound constrained problems, the line-search step performs a projected search.  That is, 
    the line search approximately minimizes the 1D objective function 
    \f$\phi_k(t) = f(P_{[a,b]}(x_k+ts_k))\f$ where \f$P_{[a,b]}\f$ denotes the projection onto 
    the upper and lower bounds.  Such line-search algorithms correspond to projected gradient 
    and projected Newton-type algorithms.  

    For projected methods, we require the notion of an active set of an iterate \f$x_k\f$, 
    \f[
       \mathcal{A}_k = \{\, \xi\in\Xi\,:\,x_k(\xi) = a(\xi)\,\}\cap
                       \{\, \xi\in\Xi\,:\,x_k(\xi) = b(\xi)\,\}.
    \f]
    Given \f$\mathcal{A}_k\f$ and a search direction \f$s_k\f$, we define the binding set as
    \f[
       \mathcal{B}_k = \{\, \xi\in\Xi\,:\,x_k(\xi) = a(\xi) \;\text{and}\; s_k(\xi) < 0 \,\}\cap
                       \{\, \xi\in\Xi\,:\,x_k(\xi) = b(\xi) \;\text{and}\; s_k(\xi) > 0 \,\}.
    \f]
    The binding set contains the values of \f$\xi\in\Xi\f$ such that if \f$x_k(\xi)\f$ is on a 
    bound, then \f$(x_k+s_k)(\xi)\f$ will violate bound.  Using these definitions, we can 
    redefine the sufficient decrease and curvature conditions from the unconstrained case to 
    the case of bound constraints.

    LineSearchStep implements a number of algorithms for both bound constrained and unconstrained 
    optimization.  These algorithms are: Steepest descent; Nonlinear CG; Quasi-Newton methods;
    Inexact Newton methods; Newton's method. These methods are chosen through the EDescent enum.
*/


namespace ROL {

template <class Real>
class LineSearchStep : public Step<Real> {
private:

  Teuchos::RCP<Step<Real> >        desc_;       ///< Unglobalized step object
  Teuchos::RCP<Secant<Real> >      secant_;     ///< Secant object (used for quasi-Newton)
  Teuchos::RCP<Krylov<Real> >      krylov_;     ///< Krylov solver object (used for inexact Newton)
  Teuchos::RCP<NonlinearCG<Real> > nlcg_;       ///< Nonlinear CG object (used for nonlinear CG)
  Teuchos::RCP<LineSearch<Real> >  lineSearch_; ///< Line-search object

  Teuchos::RCP<Vector<Real> > d_;

  ELineSearch         els_;   ///< enum determines type of line search
  ECurvatureCondition econd_; ///< enum determines type of curvature condition
 
  int ls_nfval_; ///< Number of function evaluations during line search
  int ls_ngrad_; ///< Number of gradient evaluations during line search

  bool acceptLastAlpha_;  ///< For backwards compatibility. When max function evaluations are reached take last step

  int verbosity_;

  Teuchos::ParameterList parlist_;

  Real GradDotStep(const Vector<Real> &g, const Vector<Real> &s,
                   const Vector<Real> &x,
                   BoundConstraint<Real> &bnd, Real eps = 0.0) {
    Real gs = 0.0;
    if (!bnd.isActivated()) {
      gs = s.dot(g.dual());
    }
    else {
      d_->set(s);
      bnd.pruneActive(*d_,g,x,eps);
      gs = d_->dot(g.dual());
      d_->set(x);
      d_->axpy(-1.0,g.dual());
      bnd.project(*d_);
      d_->scale(-1.0);
      d_->plus(x);
      bnd.pruneInactive(*d_,g,x,eps);
      gs -= d_->dot(g.dual());
    }
    return gs;
  }

public:

  using Step<Real>::initialize;
  using Step<Real>::compute;
  using Step<Real>::update;

  /** \brief Constructor.

      Standard constructor to build a LineSearchStep object.  Algorithmic 
      specifications are passed in through a Teuchos::ParameterList.  The
      line-search type, secant type, Krylov type, or nonlinear CG type can
      be set using user-defined objects.

      @param[in]     parlist    is a parameter list containing algorithmic specifications
      @param[in]     lineSearch is a user-defined line search object
      @param[in]     secant     is a user-defined secant object
      @param[in]     krylov     is a user-defined Krylov object
      @param[in]     nlcg       is a user-defined Nonlinear CG object
  */
  LineSearchStep( Teuchos::ParameterList &parlist,
                  const Teuchos::RCP<LineSearch<Real> > &lineSearch = Teuchos::null,
                  const Teuchos::RCP<Secant<Real> > &secant = Teuchos::null,
                  const Teuchos::RCP<Krylov<Real> > &krylov = Teuchos::null,
                  const Teuchos::RCP<NonlinearCG<Real> > &nlcg = Teuchos::null )
    : Step<Real>(), desc_(Teuchos::null), secant_(secant),
      krylov_(krylov), nlcg_(nlcg), lineSearch_(lineSearch),
      els_(LINESEARCH_USERDEFINED), econd_(CURVATURECONDITION_WOLFE),
      ls_nfval_(0), ls_ngrad_(0), verbosity_(0), parlist_(parlist) {
    // Parse parameter list
    Teuchos::ParameterList& Llist = parlist.sublist("Step").sublist("Line Search");
    Teuchos::ParameterList& Glist = parlist.sublist("General");
    econd_ = StringToECurvatureCondition(Llist.sublist("Curvature Condition").get("Type","Strong Wolfe Conditions") );
    acceptLastAlpha_ = Llist.get("Accept Last Alpha", false); 
    verbosity_ = Glist.get("Print Verbosity",0);
    // Initialize Line Search
    if (lineSearch_ == Teuchos::null) {
      els_ = StringToELineSearch(Llist.sublist("Line-Search Method").get("Type","Cubic Interpolation") );
      lineSearch_ = LineSearchFactory<Real>(parlist);
    }
  }

//  /** \brief Constructor.
//
//      Standard constructor to build a LineSearchStep object.  Algorithmic 
//      specifications are passed in through a Teuchos::ParameterList.
//
//      @param[in]     lineSearch is a user-defined line search object
//      @param[in]     parlist    is a parameter list containing algorithmic specifications
//  */
//  LineSearchStep( Teuchos::ParameterList &parlist,
//                  const Teuchos::RCP<LineSearch<Real> > &lineSearch )
//    : Step<Real>(), desc_(Teuchos::null), secant_(Teuchos::null),
//      krylov_(Teuchos::null), nlcg_(Teuchos::null), lineSearch_(lineSearch),
//      els_(LINESEARCH_USERDEFINED), econd_(CURVATURECONDITION_WOLFE),
//      ls_nfval_(0), ls_ngrad_(0), verbosity_(0), parlist_(parlist) {
//    // Parse parameter list
//    Teuchos::ParameterList& Llist = parlist.sublist("Step").sublist("Line Search");
//    Teuchos::ParameterList& Glist = parlist.sublist("General");
//    econd_ = StringToECurvatureCondition(Llist.sublist("Curvature Condition").get("Type","Strong Wolfe Conditions") );
//    acceptLastAlpha_ = Llist.get("Accept Last Alpha", false); 
//    verbosity_ = Glist.get("Print Verbosity",0);
//  }
//
//  /** \brief Constructor.
//
//      Constructor to build a LineSearchStep object with a user-defined 
//      secant object.  Algorithmic specifications are passed in through 
//      a Teuchos::ParameterList.
//
//      @param[in]     secant     is a user-defined secant object
//      @param[in]     parlist    is a parameter list containing algorithmic specifications
//  */
//  LineSearchStep( Teuchos::ParameterList &parlist,
//                  const Teuchos::RCP<Secant<Real> > &secant )
//    : Step<Real>(), desc_(Teuchos::null), secant_(secant),
//      krylov_(Teuchos::null), nlcg_(Teuchos::null), lineSearch_(Teuchos::null),
//      els_(LINESEARCH_BACKTRACKING), econd_(CURVATURECONDITION_WOLFE),
//      ls_nfval_(0), ls_ngrad_(0), verbosity_(0), parlist_(parlist) {
//    // Parse parameter list
//    Teuchos::ParameterList& Llist = parlist.sublist("Step").sublist("Line Search");
//    Teuchos::ParameterList& Glist = parlist.sublist("General");
//    els_ = StringToELineSearch(Llist.sublist("Line-Search Method").get("Type","Cubic Interpolation") );
//    econd_ = StringToECurvatureCondition(Llist.sublist("Curvature Condition").get("Type","Strong Wolfe Conditions") );
//    acceptLastAlpha_ = Llist.get("Accept Last Alpha", false); 
//    verbosity_ = Glist.get("Print Verbosity",0);
//    // Initialize Line Search
//    lineSearch_ = LineSearchFactory<Real>(parlist);
//  }
//
//  /** \brief Constructor.
//
//      Standard constructor to build a LineSearchStep object.  Algorithmic 
//      specifications are passed in through a Teuchos::ParameterList.
//
//      @param[in]     krylov     is a user-defined Krylov object
//      @param[in]     parlist    is a parameter list containing algorithmic specifications
//  */
//  LineSearchStep( Teuchos::ParameterList &parlist,
//                  const Teuchos::RCP<Krylov<Real> > &krylov )
//    : Step<Real>(), desc_(Teuchos::null), secant_(Teuchos::null),
//      krylov_(krylov), nlcg_(Teuchos::null), lineSearch_(Teuchos::null),
//      els_(LINESEARCH_BACKTRACKING), econd_(CURVATURECONDITION_WOLFE),
//      ls_nfval_(0), ls_ngrad_(0), verbosity_(0), parlist_(parlist) {
//    // Parse parameter list
//    Teuchos::ParameterList& Llist = parlist.sublist("Step").sublist("Line Search");
//    Teuchos::ParameterList& Glist = parlist.sublist("General");
//    els_ = StringToELineSearch(Llist.sublist("Line-Search Method").get("Type","Cubic Interpolation") );
//    econd_ = StringToECurvatureCondition(Llist.sublist("Curvature Condition").get("Type","Strong Wolfe Conditions") );
//    acceptLastAlpha_ = Llist.get("Accept Last Alpha", false);
//    verbosity_ = Glist.get("Print Verbosity",0);
//    // Initialize Line Search
//    lineSearch_ = LineSearchFactory<Real>(parlist);
//  }
//
//  /** \brief Constructor.
//
//      Constructor to build a LineSearchStep object with a user-defined 
//      secant object.  Algorithmic specifications are passed in through 
//      a Teuchos::ParameterList.
//
//      @param[in]     lineSearch is a user-defined line search object
//      @param[in]     secant     is a user-defined secant object
//      @param[in]     parlist    is a parameter list containing algorithmic specifications
//  */
//  LineSearchStep( Teuchos::ParameterList &parlist,
//                  const Teuchos::RCP<LineSearch<Real> > &lineSearch,
//                  const Teuchos::RCP<Secant<Real> > &secant )
//    : Step<Real>(), desc_(Teuchos::null), secant_(secant),
//      krylov_(Teuchos::null), nlcg_(Teuchos::null), lineSearch_(lineSearch),
//      els_(LINESEARCH_USERDEFINED), econd_(CURVATURECONDITION_WOLFE),
//      ls_nfval_(0), ls_ngrad_(0), verbosity_(0), parlist_(parlist) {
//    // Parse parameter list
//    Teuchos::ParameterList& Llist = parlist.sublist("Step").sublist("Line Search");
//    Teuchos::ParameterList& Glist = parlist.sublist("General");
//    econd_ = StringToECurvatureCondition(Llist.sublist("Curvature Condition").get("Type","Strong Wolfe Conditions") );
//    acceptLastAlpha_ = Llist.get("Accept Last Alpha", false);
//    verbosity_ = Glist.get("Print Verbosity",0);
//  }

  void initialize( Vector<Real> &x, const Vector<Real> &s, const Vector<Real> &g, 
                   Objective<Real> &obj, BoundConstraint<Real> &bnd, 
                   AlgorithmState<Real> &algo_state ) {
    d_ = x.clone();

    // Initialize unglobalized step
    EDescent edesc = StringToEDescent(parlist_.sublist("Step").sublist("Line Search").sublist("Descent Method").get("Type","Quasi-Newton Method") );
    if (bnd.isActivated()) {
      switch(edesc) {
        case DESCENT_STEEPEST:
          desc_ = Teuchos::rcp(new GradientStep<Real>(parlist_));                              break;
        case DESCENT_NONLINEARCG:
          desc_ = Teuchos::rcp(new NonlinearCGStep<Real>(parlist_,nlcg_));                     break;
        case DESCENT_SECANT:
          desc_ = Teuchos::rcp(new ProjectedSecantStep<Real>(parlist_,secant_));               break;
        case DESCENT_NEWTON:
          desc_ = Teuchos::rcp(new ProjectedNewtonStep<Real>(parlist_));                       break;
        case DESCENT_NEWTONKRYLOV:
          desc_ = Teuchos::rcp(new ProjectedNewtonKrylovStep<Real>(parlist_,krylov_,secant_)); break;
        default:
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::invalid_argument,
            ">>> (LineSearchStep::Initialize): Undefined descent type!");
      }
    }
    else {
      switch(edesc) {
        case DESCENT_STEEPEST:
          desc_ = Teuchos::rcp(new GradientStep<Real>(parlist_));                     break;
        case DESCENT_NONLINEARCG:
          desc_ = Teuchos::rcp(new NonlinearCGStep<Real>(parlist_,nlcg_));            break;
        case DESCENT_SECANT:
          desc_ = Teuchos::rcp(new SecantStep<Real>(parlist_,secant_));               break;
        case DESCENT_NEWTON:
          desc_ = Teuchos::rcp(new NewtonStep<Real>(parlist_));                       break;
        case DESCENT_NEWTONKRYLOV:
          desc_ = Teuchos::rcp(new NewtonKrylovStep<Real>(parlist_,krylov_,secant_)); break;
        default:
          TEUCHOS_TEST_FOR_EXCEPTION(true,std::invalid_argument,
            ">>> (LineSearchStep::Initialize): Undefined descent type!");
      }
    }
    desc_->initialize(x,s,g,obj,bnd,algo_state);

    // Initialize line search
    lineSearch_->initialize(x,s,g,obj,bnd);
    //Teuchos::RCP<const StepState<Real> > desc_state = desc_->getStepState();
    //lineSearch_->initialize(x,s,*(desc_state->gradientVec),obj,bnd);
  }

  /** \brief Compute step.

      Computes a trial step, \f$s_k\f$ as defined by the enum EDescent.  Once the 
      trial step is determined, this function determines an approximate minimizer 
      of the 1D function \f$\phi_k(t) = f(x_k+ts_k)\f$.  This approximate 
      minimizer must satisfy sufficient decrease and curvature conditions.

      @param[out]      s          is the computed trial step
      @param[in]       x          is the current iterate
      @param[in]       obj        is the objective function
      @param[in]       bnd        are the bound constraints
      @param[in]       algo_state contains the current state of the algorithm
  */
  void compute( Vector<Real> &s, const Vector<Real> &x,
                Objective<Real> &obj, BoundConstraint<Real> &bnd, 
                AlgorithmState<Real> &algo_state ) {
    // Compute unglobalized step
    desc_->compute(s,x,obj,bnd,algo_state);

    // Ensure that s is a descent direction
    // ---> If not, then default to steepest descent
    Teuchos::RCP<const StepState<Real> > desc_state = desc_->getStepState();
    Real gs = GradDotStep(*(desc_state->gradientVec),s,x,bnd,algo_state.gnorm);
    if (gs >= 0.0) {
      s.set((desc_state->gradientVec)->dual());
      s.scale(-1.0);
      gs = GradDotStep(*(desc_state->gradientVec),s,x,bnd,algo_state.gnorm);
    }

    // Perform line search
    Teuchos::RCP<StepState<Real> > step_state = Step<Real>::getState();
    Real fnew = algo_state.value;
    ls_nfval_ = 0; ls_ngrad_ = 0;
    lineSearch_->setData(algo_state.gnorm,*(desc_state->gradientVec));
    lineSearch_->run(step_state->searchSize,fnew,ls_nfval_,ls_ngrad_,gs,s,x,obj,bnd);
    algo_state.nfval += ls_nfval_;
    algo_state.ngrad += ls_ngrad_;

    // Make correction if maximum function evaluations reached
    if(!acceptLastAlpha_) {  
      lineSearch_->setMaxitUpdate(step_state->searchSize,fnew,algo_state.value);
    }
    algo_state.value = fnew;

    // Compute scaled descent direction
    s.scale(step_state->searchSize);
  }

  /** \brief Update step, if successful.

      Given a trial step, \f$s_k\f$, this function updates \f$x_{k+1}=x_k+s_k\f$. 
      This function also updates the secant approximation.

      @param[in,out]   x          is the updated iterate
      @param[in]       s          is the computed trial step
      @param[in]       obj        is the objective function
      @param[in]       con        are the bound constraints
      @param[in]       algo_state contains the current state of the algorithm
  */
  void update( Vector<Real> &x, const Vector<Real> &s,
               Objective<Real> &obj, BoundConstraint<Real> &bnd,
               AlgorithmState<Real> &algo_state ) {
    desc_->update(x,s,obj,bnd,algo_state);
  }

  /** \brief Print iterate header.

      This function produces a string containing header information.
  */
  std::string printHeader( void ) const {
    std::string head = desc_->printHeader();
    head.erase(std::remove(head.end()-3,head.end(),'\n'), head.end());
    std::stringstream hist;
    hist.write(head.c_str(),head.length());
    hist << std::setw(10) << std::left << "ls_#fval";
    hist << std::setw(10) << std::left << "ls_#grad";
    hist << "\n";
    return hist.str();
  }
  
  /** \brief Print step name.

      This function produces a string containing the algorithmic step information.
  */
  std::string printName( void ) const {
    std::string name = desc_->printName();
    std::stringstream hist;
    hist << name;
    hist << "Line Search: " << ELineSearchToString(els_);
    hist << " satisfying " << ECurvatureConditionToString(econd_) << "\n";
    return hist.str();
  }

  /** \brief Print iterate status.

      This function prints the iteration status.

      @param[in]     algo_state    is the current state of the algorithm
      @param[in]     printHeader   if ste to true will print the header at each iteration
  */
  std::string print( AlgorithmState<Real> & algo_state, bool print_header = false ) const  {
    std::string desc = desc_->print(algo_state,false);
    desc.erase(std::remove(desc.end()-3,desc.end(),'\n'), desc.end());
    std::string name = desc_->printName();
    size_t pos = desc.find(name);
    if ( pos != std::string::npos ) {
      desc.erase(pos, name.length());
    }

    std::stringstream hist;
    if ( algo_state.iter == 0 ) {
      hist << printName();
    }
    if ( print_header ) {
      hist << printHeader();
    }
    hist << desc;
    if ( algo_state.iter == 0 ) {
      hist << "\n";
    }
    else {
      hist << std::setw(10) << std::left << ls_nfval_;              
      hist << std::setw(10) << std::left << ls_ngrad_;              
      hist << "\n";
    }
    return hist.str();
  }
}; // class LineSearchStep

} // namespace ROL

#endif
