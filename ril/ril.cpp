#define LOG_TAG "EF67RIL"

#include <dlfcn.h>
#include <log/log.h>
#include <stddef.h>
#include <telephony/ril.h>

#include <mutex>
#include <set>

// S0226114 app status includes four PIN/PUK retry counters after RIL_AppStatus.
struct StockAppStatus {
    RIL_AppStatus status;
    int remaining_count_pin1;
    int remaining_count_puk1;
    int remaining_count_pin2;
    int remaining_count_puk2;
};

struct StockCardStatus {
    RIL_CardState card_state;
    RIL_PinState universal_pin_state;
    int gsm_umts_subscription_app_index;
    int cdma_subscription_app_index;
    int ims_subscription_app_index;
    int num_applications;
    StockAppStatus applications[RIL_CARD_MAX_APPS];
};

static_assert(sizeof(StockCardStatus) == 408, "EF67 stock card status ABI");
static_assert(sizeof(RIL_CardStatus_v6) == 280, "32-bit Android card status ABI");

static const RIL_Env *framework_env;
static RIL_Env stock_env;
static const RIL_RadioFunctions *stock_functions;
static RIL_RadioFunctions radio_functions;
static std::mutex sim_requests_mutex;
static std::set<RIL_Token> sim_requests;

static void on_request_complete(RIL_Token token, RIL_Errno error,
        void *response, size_t length)
{
    bool sim_status;
    {
        std::lock_guard<std::mutex> lock(sim_requests_mutex);
        sim_status = sim_requests.erase(token) != 0;
    }
    if (!sim_status || error != RIL_E_SUCCESS) {
        framework_env->OnRequestComplete(token, error, response, length);
        return;
    }

    const StockCardStatus *stock = static_cast<const StockCardStatus *>(response);
    if (!stock || length != sizeof(*stock) || stock->num_applications < 0 ||
            stock->num_applications > RIL_CARD_MAX_APPS) {
        ALOGE("Invalid stock SIM status response");
        framework_env->OnRequestComplete(token, RIL_E_GENERIC_FAILURE, nullptr, 0);
        return;
    }
    RIL_CardStatus_v6 status = {};
    status.card_state = stock->card_state;
    status.universal_pin_state = stock->universal_pin_state;
    status.gsm_umts_subscription_app_index = stock->gsm_umts_subscription_app_index;
    status.cdma_subscription_app_index = stock->cdma_subscription_app_index;
    status.ims_subscription_app_index = stock->ims_subscription_app_index;
    status.num_applications = stock->num_applications;
    for (int i = 0; i < status.num_applications; ++i) {
        status.applications[i] = stock->applications[i].status;
    }
    framework_env->OnRequestComplete(token, RIL_E_SUCCESS, &status, sizeof(status));
}

static void on_request(int request, void *data, size_t length, RIL_Token token)
{
    // These newer Android requests collide with vendor commands in S0226114.
    if (request == RIL_REQUEST_GET_RADIO_CAPABILITY || request == RIL_REQUEST_START_LCE ||
            request == RIL_REQUEST_GET_ACTIVITY_INFO) {
        framework_env->OnRequestComplete(token, RIL_E_REQUEST_NOT_SUPPORTED, nullptr, 0);
        return;
    }
    if (request == RIL_REQUEST_GET_SIM_STATUS) {
        std::lock_guard<std::mutex> lock(sim_requests_mutex);
        sim_requests.insert(token);
    }
    stock_functions->onRequest(request, data, length, token);
}

extern "C" const RIL_RadioFunctions *RIL_Init(const RIL_Env *env, int argc, char **argv)
{
    void *handle = dlopen("/system/vendor/lib/libril-qc-qmi-1.so", RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        ALOGE("Cannot load stock RIL: %s", dlerror());
        return nullptr;
    }
    auto stock_init = reinterpret_cast<decltype(&RIL_Init)>(dlsym(handle, "RIL_Init"));
    if (!stock_init) {
        ALOGE("Cannot find stock RIL_Init: %s", dlerror());
        dlclose(handle);
        return nullptr;
    }
    framework_env = env;
    stock_env = *env;
    stock_env.OnRequestComplete = on_request_complete;
    stock_functions = stock_init(&stock_env, argc, argv);
    if (!stock_functions) {
        dlclose(handle);
        return nullptr;
    }
    radio_functions = *stock_functions;
    radio_functions.onRequest = on_request;
    return &radio_functions;
}
