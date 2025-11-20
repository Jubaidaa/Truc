#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../src/logger/logger.h"

static void test_logger_create_with_stdout(void)
{
    printf("Test: logger_create_with_stdout\n");

    struct server_config config = { 0 };
    config.log_enabled = true;
    config.server_name = "testserver";
    config.log_file = NULL;

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);
    assert(logger->enabled == true);
    assert(logger->output == stdout);
    assert(logger->owns_file == false);
    assert(strcmp(logger->server_name, "testserver") == 0);

    logger_destroy(logger);
    printf("  PASSED\n");
}

static void test_logger_create_with_file(void)
{
    printf("Test: logger_create_with_file\n");

    const char *test_log = "/tmp/test_logger.log";
    unlink(test_log);

    struct server_config config = { 0 };
    config.log_enabled = true;
    config.server_name = "fileserver";
    config.log_file = test_log;

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);
    assert(logger->enabled == true);
    assert(logger->output != NULL);
    assert(logger->output != stdout);
    assert(logger->owns_file == true);

    logger_destroy(logger);
    unlink(test_log);
    printf("  PASSED\n");
}

static void test_logger_disabled(void)
{
    printf("Test: logger_disabled\n");

    struct server_config config = { 0 };
    config.log_enabled = false;
    config.server_name = "disabled";

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);
    assert(logger->enabled == false);

    logger_destroy(logger);
    printf("  PASSED\n");
}

static void test_logger_log_request(void)
{
    printf("Test: logger_log_request\n");

    const char *test_log = "/tmp/test_request.log";
    unlink(test_log);

    struct server_config config = { 0 };
    config.log_enabled = true;
    config.server_name = "testserver";
    config.log_file = test_log;

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);

    logger_log_request(logger, "GET", "/index.html", "127.0.0.1");
    logger_destroy(logger);

    FILE *f = fopen(test_log, "r");
    assert(f != NULL);

    char buffer[512];
    char *result = fgets(buffer, sizeof(buffer), f);
    assert(result != NULL);

    assert(strstr(buffer, "[testserver]") != NULL);
    assert(strstr(buffer, "received GET on '/index.html'") != NULL);
    assert(strstr(buffer, "from 127.0.0.1") != NULL);
    assert(strstr(buffer, "GMT") != NULL);

    fclose(f);
    unlink(test_log);
    printf("  PASSED\n");
}

static void test_logger_log_bad_request(void)
{
    printf("Test: logger_log_bad_request\n");

    const char *test_log = "/tmp/test_bad_request.log";
    unlink(test_log);

    struct server_config config = { 0 };
    config.log_enabled = true;
    config.server_name = "testserver";
    config.log_file = test_log;

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);

    logger_log_bad_request(logger, "192.168.1.1");
    logger_destroy(logger);

    FILE *f = fopen(test_log, "r");
    assert(f != NULL);

    char buffer[512];
    char *result = fgets(buffer, sizeof(buffer), f);
    assert(result != NULL);

    assert(strstr(buffer, "[testserver]") != NULL);
    assert(strstr(buffer, "received Bad Request") != NULL);
    assert(strstr(buffer, "from 192.168.1.1") != NULL);
    assert(strstr(buffer, "GMT") != NULL);

    fclose(f);
    unlink(test_log);
    printf("  PASSED\n");
}

static void test_logger_log_response(void)
{
    printf("Test: logger_log_response\n");

    const char *test_log = "/tmp/test_response.log";
    unlink(test_log);

    struct server_config config = { 0 };
    config.log_enabled = true;
    config.server_name = "testserver";
    config.log_file = test_log;

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);

    logger_log_response(logger, 200, "127.0.0.1", "GET", "/index.html");
    logger_destroy(logger);

    FILE *f = fopen(test_log, "r");
    assert(f != NULL);

    char buffer[512];
    char *result = fgets(buffer, sizeof(buffer), f);
    assert(result != NULL);

    assert(strstr(buffer, "[testserver]") != NULL);
    assert(strstr(buffer, "responding with 200") != NULL);
    assert(strstr(buffer, "to 127.0.0.1") != NULL);
    assert(strstr(buffer, "for GET on '/index.html'") != NULL);
    assert(strstr(buffer, "GMT") != NULL);

    fclose(f);
    unlink(test_log);
    printf("  PASSED\n");
}

static void test_logger_log_bad_response(void)
{
    printf("Test: logger_log_bad_response\n");

    const char *test_log = "/tmp/test_bad_response.log";
    unlink(test_log);

    struct server_config config = { 0 };
    config.log_enabled = true;
    config.server_name = "testserver";
    config.log_file = test_log;

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);

    logger_log_bad_response(logger, 400, "192.168.1.1");
    logger_destroy(logger);

    FILE *f = fopen(test_log, "r");
    assert(f != NULL);

    char buffer[512];
    char *result = fgets(buffer, sizeof(buffer), f);
    assert(result != NULL);

    assert(strstr(buffer, "[testserver]") != NULL);
    assert(strstr(buffer, "responding with 400") != NULL);
    assert(strstr(buffer, "to 192.168.1.1") != NULL);
    assert(strstr(buffer, "GMT") != NULL);

    fclose(f);
    unlink(test_log);
    printf("  PASSED\n");
}

static void test_logger_no_log_when_disabled(void)
{
    printf("Test: logger_no_log_when_disabled\n");

    struct server_config config = { 0 };
    config.log_enabled = false;
    config.server_name = "disabled";

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);

    logger_log_request(logger, "GET", "/test", "127.0.0.1");
    logger_log_response(logger, 200, "127.0.0.1", "GET", "/test");

    logger_destroy(logger);
    printf("  PASSED\n");
}

static void test_logger_default_server_name(void)
{
    printf("Test: logger_default_server_name\n");

    const char *test_log = "/tmp/test_default_name.log";
    unlink(test_log);

    struct server_config config = { 0 };
    config.log_enabled = true;
    config.server_name = NULL;
    config.log_file = test_log;

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);
    assert(strcmp(logger->server_name, "localhost") == 0);

    logger_log_request(logger, "GET", "/", "127.0.0.1");
    logger_destroy(logger);

    FILE *f = fopen(test_log, "r");
    assert(f != NULL);

    char buffer[512];
    char *result = fgets(buffer, sizeof(buffer), f);
    assert(result != NULL);
    assert(strstr(buffer, "[localhost]") != NULL);

    fclose(f);
    unlink(test_log);
    printf("  PASSED\n");
}

static void test_logger_multiple_logs(void)
{
    printf("Test: logger_multiple_logs\n");

    const char *test_log = "/tmp/test_multiple.log";
    unlink(test_log);

    struct server_config config = { 0 };
    config.log_enabled = true;
    config.server_name = "multiserver";
    config.log_file = test_log;

    struct logger *logger = logger_create(&config);
    assert(logger != NULL);

    logger_log_request(logger, "GET", "/page1", "127.0.0.1");
    logger_log_response(logger, 200, "127.0.0.1", "GET", "/page1");
    logger_log_request(logger, "POST", "/data", "192.168.1.1");
    logger_log_response(logger, 404, "192.168.1.1", "POST", "/data");
    logger_log_bad_request(logger, "10.0.0.1");
    logger_log_bad_response(logger, 400, "10.0.0.1");

    logger_destroy(logger);

    FILE *f = fopen(test_log, "r");
    assert(f != NULL);

    int line_count = 0;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), f) != NULL)
    {
        line_count++;
        assert(strstr(buffer, "[multiserver]") != NULL);
        assert(strstr(buffer, "GMT") != NULL);
    }

    assert(line_count == 6);

    fclose(f);
    unlink(test_log);
    printf("  PASSED\n");
}

int main(void)
{
    printf("Running logger tests...\n\n");

    test_logger_create_with_stdout();
    test_logger_create_with_file();
    test_logger_disabled();
    test_logger_log_request();
    test_logger_log_bad_request();
    test_logger_log_response();
    test_logger_log_bad_response();
    test_logger_no_log_when_disabled();
    test_logger_default_server_name();
    test_logger_multiple_logs();

    printf("\nAll logger tests passed!\n");
    return 0;
}
