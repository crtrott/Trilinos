// @HEADER
// ****************************************************************************
//                Tempus: Copyright (2017) Sandia Corporation
//
// Distributed under BSD 3-clause license (See accompanying file Copyright.txt)
// ****************************************************************************
// @HEADER

#ifndef Tempus_TimeStepControlStrategy_hpp
#define Tempus_TimeStepControlStrategy_hpp

// Teuchos
#include "Teuchos_ParameterListAcceptorDefaultBase.hpp"

#include "Tempus_SolutionHistory.hpp"


namespace Tempus {

template<class Scalar> class TimeStepControl;

/** \brief StepControlStrategy class for TimeStepControl
 *
 */
template<class Scalar>
class TimeStepControlStrategy
  : virtual public Teuchos::ParameterListAcceptor
{
public:

  /// Constructor
  TimeStepControlStrategy(){}

  /// Destructor
  virtual ~TimeStepControlStrategy(){}

  /// Observe Stepper at beginning of takeStep.
  virtual void getNextTimeStep(TimeStepControl<Scalar> tsc, Teuchos::RCP<SolutionHistory<Scalar> > sh ,
        Status & integratorStatus){}

  /// \name Overridden from Teuchos::ParameterListAcceptor
  //@{
    void setParameterList(const Teuchos::RCP<Teuchos::ParameterList> & pl){}
    Teuchos::RCP<const Teuchos::ParameterList> getValidParameters() const {}
    Teuchos::RCP<Teuchos::ParameterList> getNonconstParameterList() {}
    Teuchos::RCP<Teuchos::ParameterList> unsetParameterList() {}
  //@}
};
} // namespace Tempus
#endif // Tempus_TimeStepControlStrategy_hpp
