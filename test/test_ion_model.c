#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <gmcmc/gmcmc_errno.h>
#include <gmcmc/gmcmc_ion_model.h>
#include <stdlib.h>

static gmcmc_dataset * data;
static gmcmc_model * model;

static int cleanup() {
  gmcmc_ion_model * ion_model = (gmcmc_ion_model *)gmcmc_model_get_modelspecific(model);
  gmcmc_ion_model_destroy(ion_model);
  gmcmc_model_destroy(model);
  gmcmc_dataset_destroy(data);
  return 0;
}

static void castillo_katz(const double * params, double * Q, size_t ldq) {
  // Rename for clarity
  double K_1   = pow(10.0, params[0]);
  double K_2   = pow(10.0, params[1]);
  double Beta  = pow(10.0, params[2]);
  double Alpha = pow(10.0, params[3]);

  // Construct Q matrix
  // 3 states for this model
  Q[0] = -K_1;
  Q[1] =  K_2;
  Q[2] =  0.0;

  Q[ldq + 0] =  K_1;
  Q[ldq + 1] = -K_2 - Beta;
  Q[ldq + 2] = Alpha;

  Q[2 * ldq + 0] =  0.0;
  Q[2 * ldq + 1] = Beta;
  Q[2 * ldq + 2] = -Alpha;
}

static int init_castillo_katz() {
  int error;

  // Load the dataset
  if ((error = gmcmc_dataset_create_matlab_ion(&data, "../data/ION_dCK_0,5s.mat")) != 0)
    GMCMC_ERROR("Failed to create Castillo Katz dataset", error);

  // Create the priors for each of the parameters
  gmcmc_distribution ** priors;
  if ((priors = malloc(4 * sizeof(gmcmc_distribution *))) == NULL) {
    gmcmc_dataset_destroy(data);
    GMCMC_ERROR("Failed to allocate priors", GMCMC_ENOMEM);
  }

  for (int i = 0; i < 4; i++) {
    if ((error = gmcmc_distribution_create_uniform(&priors[i], -2.0, 4.0)) != 0) {
      for (int j = 0; j < i; j++)
        gmcmc_distribution_destroy(priors[j]);
      free(priors);
      gmcmc_dataset_destroy(data);
      GMCMC_ERROR("Failed to create prior", error);
    }
  }

  // Create the model
  if ((error = gmcmc_model_create(&model, 4, priors, gmcmc_ion_proposal_mh, gmcmc_ion_likelihood_mh)) != 0) {
    for (int j = 0; j < 4; j++)
      gmcmc_distribution_destroy(priors[j]);
    free(priors);
    gmcmc_dataset_destroy(data);
    GMCMC_ERROR("Failed to create Castillo Katz model", error);
  }

  // Don't need priors any more
  for (int j = 0; j < 4; j++)
    gmcmc_distribution_destroy(priors[j]);
  free(priors);

  // Set initial parameter values
  double params[] = { 2.0, 2.0, 3.0, 3.0 };
  if ((error = gmcmc_model_set_params(model, params)) != 0) {
    gmcmc_model_destroy(model);
    gmcmc_dataset_destroy(data);
    GMCMC_ERROR("Failed to set initial parameters", error);
  }

  // Set initial step size
  gmcmc_model_set_stepsize(model, 1.0);

  // Create ion model specific stuff
  gmcmc_ion_model * ion_model;
  if ((error = gmcmc_ion_model_create(&ion_model, 2, 1, castillo_katz)) != 0) {
    gmcmc_dataset_destroy(data);
    gmcmc_model_destroy(model);
    GMCMC_ERROR("Unable to create Ion Channel specific model", error);
  }

  gmcmc_model_set_modelspecific(model, ion_model);

  return 0;
}

static void test_castillo_katz_ion_proposal_mh0() {
  // Input arguments
  double params[] = { 2.88570752304669842, -1.54006338257304520, 2.17189240073973311, 2.70691937020932816 };
  double likelihood = 1926.08217818508683195;
  double temperature = 0.0;
  double stepsize = 1.0;

  // Output arguments
  double mean[4], covariance[4 * 4];
  size_t ldc = 4;

  // Call test function
  int error = gmcmc_proposal(model, params, likelihood, temperature, stepsize, NULL, mean, covariance, ldc);

  // Check return value
  CU_ASSERT(error == 0);

  // Check mean
  CU_ASSERT_DOUBLE_EQUAL(mean[0],  2.88570752304669842, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[1], -1.54006338257304520, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[2],  2.17189240073973311, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[3],  2.70691937020932816, 1.0e-15);

  // Check covariance
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 0], 1.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 1], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 2], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 3], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 4], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 5], 1.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 6], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 7], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 8], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 9], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[10], 1.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[11], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[12], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[13], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[14], 0.0, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[15], 1.0, 1.0e-15);
}

static void test_castillo_katz_ion_proposal_mh1() {
  // Input arguments
  double params[] = { 3.46657887197742420, 0.21221985055729120, -1.38727977182536200, -0.70527935730707281 };
  double likelihood = -890.07691930595535723;
  double temperature = 1.693508780843028e-05;
  double stepsize = 0.8;

  // Output arguments
  double mean[4], covariance[4 * 4];
  size_t ldc = 4;

  // Call test function
  int error = gmcmc_proposal(model, params, likelihood, temperature, stepsize, NULL, mean, covariance, ldc);

  // Check return value
  CU_ASSERT(error == 0);

  // Check mean
  CU_ASSERT_DOUBLE_EQUAL(mean[0],  3.46657887197742420, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[1],  0.21221985055729120, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[2], -1.38727977182536200, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[3], -0.70527935730707281, 1.0e-15);

  // Check covariance
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 0], 0.64, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 1], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 2], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 3], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 4], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 5], 0.64, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 6], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 7], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 8], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 9], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[10], 0.64, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[11], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[12], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[13], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[14], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[15], 0.64, 1.0e-15);
}

static void test_castillo_katz_ion_proposal_mh2() {
  // Input arguments
  double params[] = { 2.67622147429655843, -0.80350436045150375, 0.75516192694289952, 3.64326288442972235 };
  double likelihood = 996.86730497900316550;
  double temperature = 0.000541922809870;
  double stepsize = 1.2;

  // Output arguments
  double mean[4], covariance[4 * 4];
  size_t ldc = 4;

  // Call test function
  int error = gmcmc_proposal(model, params, likelihood, temperature, stepsize, NULL, mean, covariance, ldc);

  // Check return value
  CU_ASSERT(error == 0);

  // Check mean
  CU_ASSERT_DOUBLE_EQUAL(mean[0],  2.67622147429655843, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[1], -0.80350436045150375, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[2],  0.75516192694289952, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[3],  3.64326288442972235, 1.0e-15);

  // Check covariance
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 0], 1.44, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 1], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 2], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 3], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 4], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 5], 1.44, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 6], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 7], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 8], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 9], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[10], 1.44, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[11], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[12], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[13], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[14], 0.00, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[15], 1.44, 1.0e-15);
}

static void test_castillo_katz_ion_proposal_mh3() {
  // Input arguments
  double params[] = { -0.91429320430028871, -1.41244361420492504, 2.30616647892381454, 2.32060827609643727 };
  double likelihood = 1861.71426557036124905;
  double temperature = 0.004115226337449;
  double stepsize = 0.512;

  // Output arguments
  double mean[4], covariance[4 * 4];
  size_t ldc = 4;

  // Call test function
  int error = gmcmc_proposal(model, params, likelihood, temperature, stepsize, NULL, mean, covariance, ldc);

  // Check return value
  CU_ASSERT(error == 0);

  // Check mean
  CU_ASSERT_DOUBLE_EQUAL(mean[0], -0.91429320430028871, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[1], -1.41244361420492504, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[2],  2.30616647892381454, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[3],  2.32060827609643727, 1.0e-15);

  // Check covariance
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 0], 0.262144, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 1], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 2], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 3], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 4], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 5], 0.262144, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 6], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 7], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 8], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 9], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[10], 0.262144, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[11], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[12], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[13], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[14], 0.000000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[15], 0.262144, 1.0e-15);
}

static void test_castillo_katz_ion_proposal_mh4() {
  // Input arguments
  double params[] = { 1.24749972433837053, 2.37607895190583918, 2.91157798303740289, 3.28969863650566108 };
  double likelihood = 2028.66763919470463406;
  double temperature = 0.017341529915833;
  double stepsize = 1.44;

  // Output arguments
  double mean[4], covariance[4 * 4];
  size_t ldc = 4;

  // Call test function
  int error = gmcmc_proposal(model, params, likelihood, temperature, stepsize, NULL, mean, covariance, ldc);

  // Check return value
  CU_ASSERT(error == 0);

  // Check mean
  CU_ASSERT_DOUBLE_EQUAL(mean[0], 1.24749972433837053, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[1], 2.37607895190583918, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[2], 2.91157798303740289, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(mean[3], 3.28969863650566108, 1.0e-15);

  // Check covariance
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 0], 2.0736, 1.0e-15)
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 1], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 2], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 3], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 4], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 5], 2.0736, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 6], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 7], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 8], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[ 9], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[10], 2.0736, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[11], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[12], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[13], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[14], 0.0000, 1.0e-15);
  CU_ASSERT_DOUBLE_EQUAL(covariance[15], 2.0736, 1.0e-15);
}

static void test_castillo_katz_ion_likelihood_mh0() {
  // Input argument
  double params[] = {  0.725404224946106, -0.0630548731896562,  0.714742903826096, -0.204966058299775 };

  // Output argument
  double likelihood;

  // Call test function
  int error = gmcmc_likelihood(data, model, params, &likelihood, NULL, NULL);

  // Check return value
  CU_ASSERT(error == 0);

  // Check log likelihood
  CU_ASSERT_DOUBLE_EQUAL(likelihood, 213.414299912922587, 1.0e-12);
}

#include "test_castillo_katz.c"

#define CUNIT_ERROR(message) \
  do { \
    CU_cleanup_registry(); \
    fprintf(stderr, "%s\nCUnit error code %d in %s (%s:%d): %s\n", \
                     message, CU_get_error(), __func__, __FILE__, __LINE__, \
                     CU_get_error_msg()); \
    return CU_get_error(); \
  } while (0)

int main() {
  CU_ErrorCode error = CU_initialize_registry();
  if (error != CUE_SUCCESS)
    CUNIT_ERROR("Failed to initialise test registry");

  CU_pSuite castillo_katz = CU_add_suite("Castillo Katz", init_castillo_katz, cleanup);
  if (CU_get_error() != CUE_SUCCESS)
    CUNIT_ERROR("Failed to add suite to registry");

  CU_ADD_TEST(castillo_katz, test_castillo_katz_ion_proposal_mh0);
  if (CU_get_error() != CUE_SUCCESS)
    CUNIT_ERROR("Failed to add test to suite");

  CU_ADD_TEST(castillo_katz, test_castillo_katz_ion_proposal_mh1);
  if (CU_get_error() != CUE_SUCCESS)
    CUNIT_ERROR("Failed to add test to suite");

  CU_ADD_TEST(castillo_katz, test_castillo_katz_ion_proposal_mh2);
  if (CU_get_error() != CUE_SUCCESS)
    CUNIT_ERROR("Failed to add test to suite");

  CU_ADD_TEST(castillo_katz, test_castillo_katz_ion_proposal_mh3);
  if (CU_get_error() != CUE_SUCCESS)
    CUNIT_ERROR("Failed to add test to suite");

  CU_ADD_TEST(castillo_katz, test_castillo_katz_ion_proposal_mh4);
  if (CU_get_error() != CUE_SUCCESS)
    CUNIT_ERROR("Failed to add test to suite");

  CU_ADD_TEST(castillo_katz, test_castillo_katz_ion_likelihood_mh0);
  if (CU_get_error() != CUE_SUCCESS)
    CUNIT_ERROR("Failed to add test to suite");

#include "test_castillo_katz_main.c"

  error = CU_basic_run_tests();
  if (error != CUE_SUCCESS)
    CUNIT_ERROR("Failed to run tests");

  CU_cleanup_registry();

  return 0;
}
