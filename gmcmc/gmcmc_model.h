/*
 * gmcmc_model.h
 *
 *  Created on: 11 June 2013
 *      Author: Gary Macindoe
 */

#ifndef GMCMC_MODEL_H
#define GMCMC_MODEL_H

#include <gmcmc/gmcmc_distribution.h>
#include <gmcmc/gmcmc_dataset.h>

/**
 * A model.
 */
typedef struct gmcmc_model gmcmc_model;

/**
 * Proposal function.
 *
 * @param [in]  likelihood   likelihood value
 * @param [in]  n            size of the parameter vector
 * @param [in]  params       parameter vector
 * @param [in]  temperature  chain temperature
 * @param [in]  stepsize     parameter step size
 * @param [out] mean         mean vector
 * @param [out] covariance   covariance matrix
 * @param [in]  ldc          leading dimension of the covariance matrix
 *
 * @return 0 on success, non-zero on error.
 */
typedef int (*gmcmc_proposal_function)(const gmcmc_likelihood *, size_t, const double *, double, double, double *, double *, size_t);

/**
 * Likelihood function.
 *
 * @param [in]  model       model specific functions and data
 * @param [in]  n           size of parameter vector
 * @param [in]  params      parameter vector
 * @param [in]  data        data
 * @param [out] likelihood  likelihood object to allocate and populate
 *
 * @return 0 on success, non-zero on error.
 */
typedef int (*gmcmc_likelihood_function)(const gmcmc_model *, size_t, const double *, const gmcmc_dataset *, gmcmc_likelihood **);

/**
 * Creates a model.
 *
 * @param [out] model       the model to create
 * @param [in]  n           the number of parameters in the model
 * @param [in]  priors      an array of prior distributions for each parameter
 * @param [in]  proposal    a function to calculate the proposal mean and variance
 * @param [in]  likelihood  a function to calculate the likelihood
 *
 * @return 0 on success, non-zero on error.
 */
int gmcmc_model_create(gmcmc_model **, size_t, gmcmc_distribution **,
                       gmcmc_proposal_function, gmcmc_likelihood_function);

/**
 * Destroys a model.
 *
 * @param [in] model  the model to destroy.
 */
void gmcmc_model_destroy(gmcmc_model *);

/**
 * Gets the number of parameters in the model.
 *
 * @param [in] model  the model
 *
 * @return the number of parameters in the model.
 */
size_t gmcmc_model_get_num_params(const gmcmc_model *);

/**
 * Sets the initial parameter values.
 *
 * @param [in,out] model   the model
 * @param [in]    params  the parameter values (may be NULL)
 *
 * @return 0 on success, non-zero on error.
 */
int gmcmc_model_set_params(gmcmc_model *, const double *);

/**
 * Gets a pointer to the initial parameter values.
 *
 * @param [in] model  the model
 *
 * @return a pointer to the initial parameter values.
 */
const double * gmcmc_model_get_params(const gmcmc_model *);

/**
 * Gets a pointer to the priors for each parameter.
 *
 * @param [in] model  the model
 * @param [in] i      the parameter index
 *
 * @return the prior for the parameter or NULL if the i is out of range.
 */
const gmcmc_distribution * gmcmc_model_get_prior(const gmcmc_model *, size_t);

/**
 * Calculates the proposal mean vector and covariance matrix based on the
 * likelihood.
 *
 * @param [in]  likelihood   likelihood value
 * @param [in]  n            size of the parameter vector
 * @param [in]  params       parameter vector
 * @param [in]  temperature  chain temperature
 * @param [in]  stepsize     parameter step size
 * @param [out] mean         mean vector
 * @param [out] covariance   covariance matrix
 * @param [in]  ldc          leading dimension of the covariance matrix
 *
 * @return 0 on success, non-zero on error.
 */
int gmcmc_model_proposal(const gmcmc_likelihood *, size_t, const double *, double, double, double *, double *, size_t);

/**
 * Calculates the likelihood of the data given the model and parameters.
 *
 * @param [in]  model       the model to evaluate
 * @param [in]  n           the number of parameters in the model
 * @param [in]  params      the current parameter values to evaluate the model
 * @param [in]  data        the data
 * @param [out] likelihood  the likelihood object to allocate and populate
 *
 * @return 0 on success, non-zero on error.
 */
int gmcmc_model_likelihood(const gmcmc_model *, size_t, const double *, const gmcmc_dataset *, gmcmc_likelihood **);

/**
 * Sets the parameter stepsize.
 *
 * @param [in] model     the model
 * @param [in] stepsize  the stepsize
 *
 * @return 0 on success,
 *         GMCMC_EINVAL if the stepsize is less than or equal to zero.
 */
int gmcmc_model_set_stepsize(gmcmc_model *, double);

/**
 * Gets the parameter stepsize.
 *
 * @param [in] model  the model
 *
 * @return the parameter stepsize.
 */
double gmcmc_model_get_stepsize(const gmcmc_model *);


// int gmcmc_model_set_blocking(gmcmc_model *, int **, const int *, int);
// int gmcmc_model_get_block(const gmcmc_model *, int **, int *, int);

/**
 * Stores a pointer to any model-specific data and functions in the model.
 *
 * @param [in,out] model          the model
 * @param [in]    modelspecific  model-specific data and functions (may be NULL)
 */
void gmcmc_model_set_modelspecific(gmcmc_model *, void *);

/**
 * Gets a pointer to the model-specific data and functions.
 *
 * @param [in] model  the model
 *
 * @return a pointer to the model-specific data and functions.
 */
const void * gmcmc_model_get_modelspecific(const gmcmc_model *);

#endif /* GMCMC_MODEL_H */
