#include "http_util.h"
#include "http_request.h"
#include "http_file_handling.h"
#include "http_string_handling.h"
#include "http_database.h"
#include "http_authentication.h"
#include "thread_pool.h"

void second_page_handler(Arena* arena, HTTPRequestInfo* request_info, HTTPResponse* response) {
    printf("second_page_handler: `%s`\n", request_info->user_permission); 

    HTTP_SetContentTypeHeader("text/html");
    cJSON* elem = NULL;
    cJSON_ArrayForEach(elem, HTTP_Auth_GetAllSessionID(true, false)) {
        printf("[second_page_handler] deleted user with session ID: `%s`\n", elem->valuestring);
        HTTP_Auth_DeleteUserAtSessionToken(elem->valuestring);
    }

    HTTP_AddCookieToCookieJar(arena, &response->cookie_jar, "oloooo", "ofofjlaa", 20, NULL, "/", NULL, false, true);
    response->response_body = HTTP_StrLit("<h1>Deleted Users</h1><p>Trying to fillout some space</p>");
}

void root_page_handler(Arena* arena, HTTPRequestInfo* request_info, HTTPResponse* response) {
    printf("root_page_handler: `%s`\n", request_info->user_permission); 

    if (!strcmp(request_info->request_method, "POST") && request_info->is_json_request) {
        HTTP_RunSQLQuery("DELETE FROM Info", false, true);
        cJSON* converted_array = HTTP_cJSON_TurnObjectIntoArray("Info", request_info->json_request_body);
        HTTP_InsertJSONIntoDatabase(converted_array);
    }

    HTTP_Set404Page("static/ginger.jpg");

    cJSON* result = HTTP_RunSQLQuery("SELECT DISTINCT fname, lname, pname FROM Info", false, true);
    HTTP_SetContentTypeHeader("text/html");

    /* HTTP_AddHeaderToHeaderDict(arena, &response->headers, "Set-Cookie", "name=John"); */
    HTTP_AddCookieToCookieJar(arena, &response->cookie_jar, "good", "hello", -1, NULL, NULL, NULL, false, false);
    HTTP_AddCookieToCookieJar(arena, &response->cookie_jar, "morning", "world", -1, NULL, NULL, NULL, false, false);
    HTTP_AddCookieToCookieJar(arena, &response->cookie_jar, "some", "hi", -1, NULL, "/", NULL, false, true);

    char* first_session_id  = HTTP_Auth_AddUserIfNotExists("writer,blogger", "Umpa", "Lumpa", 15, "Cats", "Somewhere in America!", 102.149981, "something", "not good", "really not good", NULL, NULL, "good");
    char* second_session_id = HTTP_Auth_AddUserIfNotExists("user", "Google", "Carmack", 29, "Dogs", "Somewhere in the Milky Way!", "harrow!", "something else", "not good", "really not good", NULL, NULL, "good");
    /* HTTP_AddCookieToCookieJar(arena, &response->cookie_jar, "SessionID", first_session_id, -1, NULL, NULL, NULL, false, true); */
    HTTP_AddCookieToCookieJar(arena, &response->cookie_jar, "SessionID", first_session_id, -1, NULL, NULL, NULL, false, true);
    // 0 Deletes the cookie!!!
    HTTP_AddCookieToCookieJar(arena, &response->cookie_jar, "oloooo", "ofofjlaa", 20, NULL, "/", NULL, false, true);

    response->response_body = HTTP_TemplateTextFromFile(arena, request_info, result, "static/first_page.html");

    HTTP_HandleRoute(StrArrayLit("global"), "GET", "/ooga", false, second_page_handler);
    HTTP_DeleteRouteForAllMethod("/ooga", false);
}

void login_form_handler(Arena* arena, HTTPRequestInfo* request_info, HTTPResponse* response) {
    printf("login_form_handler: `%s`\n", request_info->user_permission); 
    printf("got into login form handler!");

    HTTP_SetContentTypeHeader("text/html");

    cJSON* user_details = cJSON_CreateObject();
    cJSON_AddStringToObject(user_details, "username", "Google");
    cJSON_AddStringToObject(user_details, "password", "Carmack");
    cJSON_AddStringToObject(user_details, "age", "29");

    cJSON* login_and_user_data = HTTP_Auth_GetLoginAndUserDataAtSessionToken(HTTP_Auth_GetSessionIDAtLoginDetails(user_details), true, false, true);
    printf("login and user data: `%s`\n", cJSON_Print(login_and_user_data));
    response->response_body = HTTP_TemplateTextFromFile(arena, request_info, login_and_user_data, "static/login_form.html");
}

int main(void) {
#if 0
    SSL_library_init();
    SSL_load_error_strings();

    SSL_CTX* context = SSL_CTX_new(TLS_server_method());

    if (!SSL_CTX_set_min_proto_version(context, TLS1_2_VERSION)) {
        SSL_CTX_free(context);
        printf("[ERROR] Failed to set the minimum TLS protocol version.\n");
        ERR_print_errors_fp(stderr);
    }

    SSL_CTX_set_options(context, SSL_OP_IGNORE_UNEXPECTED_EOF|SSL_OP_NO_RENEGOTIATION|SSL_OP_CIPHER_SERVER_PREFERENCE);

    char ssl_cache[1024] = {0};
    SSL_CTX_set_session_id_context(context, ssl_cache, ArrayCount(ssl_cache));
    SSL_CTX_set_session_cache_mode(context, SSL_SESS_CACHE_SERVER);
    SSL_CTX_sess_set_cache_size(context, ArrayCount(ssl_cache));
    SSL_CTX_set_timeout(context, 3600); // NOTE: 1 hour timeout
    SSL_CTX_set_verify(context, SSL_VERIFY_NONE, NULL);

    if (SSL_CTX_use_certificate_file(context, "vendor/certs/cert.pem", SSL_FILETYPE_PEM) <= 0) {
        printf("[INFO] Started loading certificate file.\n");
        ERR_print_errors_fp(stderr);
        printf("[INFO] Finished loading certificate file.\n");
        return 1;
    } 

    if (SSL_CTX_use_PrivateKey_file(context, "vendor/certs/key.pem", SSL_FILETYPE_PEM) <= 0) {
        printf("[INFO] Started loading private key file.\n");
        ERR_print_errors_fp(stderr);
        printf("[INFO] Finished loading private key file.\n");
        return 1;
    }

    BIO* client_socket = BIO_new_accept("8000");
    if (client_socket == NULL) {
        SSL_CTX_free(context);
        printf("[ERROR] An error occured when creating the accept BIO.\n");
        ERR_print_errors_fp(stderr);
    }

    BIO_set_bind_mode(client_socket, BIO_BIND_REUSEADDR);

    if (BIO_do_accept(client_socket) <= 0) {
        SSL_CTX_free(context);
        printf("[ERROR] An error occured when setting up the client socket.\n");
        ERR_print_errors_fp(stderr);
    }

    while (true) {
        ERR_clear_error();
        if (BIO_do_accept(client_socket) <= 0) {
            continue;
        }

        BIO* client_bio = BIO_pop(client_socket);
        printf("[INFO] New client accepted.\n");

        SSL* ssl = SSL_new(context);
        if (ssl == NULL) {
            printf("[ERROR] An error occured when creating the SSL handle for the new client.\n");
            ERR_print_errors_fp(stderr);
            BIO_free(client_bio);
            continue;
        }

        SSL_set_bio(ssl, client_bio, client_bio);

        if (SSL_accept(ssl) <= 0) {
            printf("[ERROR] An error occured when performing the SSL handshake with the client.\n");
            ERR_print_errors_fp(stderr);
            SSL_free(ssl);
            continue;
        }

        char client_buffer[5024] = {0};
        char* return_data = "HTTP/1.1 200 OK\r\n\r\n<h1>Hello world</h1> ";

        while (SSL_read(ssl, client_buffer, ArrayCount(client_buffer)) > 0) {
            if (SSL_write(ssl, return_data, (int)strlen(return_data)) > 0) {
                continue;
            }

            break;
        }

        printf("[INFO] Client connection closed.\n");
        SSL_free(ssl);
    }
#endif
    if (sqlite3_threadsafe() == 0) {
        printf("[ERROR] The version of sqlite3 you are is not thread safe, please try to recompile it with the following flag: `SQLITE_THREADSAFE=1`.\n");
        return -1;
    }

    HTTP_Initialize();

    HTTP_CreateDatabase("new.db");
    HTTP_RunSQLQuery("CREATE TABLE IF NOT EXISTS Info(fname TEXT, lname TEXT, pname TEXT, custom TEXT, name TEXT, surname TEXT)", false, false);

    SessionMaxTimeout timeout = {
        .years = 10,
        .months = 20,
        .days = 40,
        .hours = 29,
        .minutes = 10,
        .seconds = 2000
    };

    HTTP_Auth_SessionEnable("SessionAuthTable",
            StrArrayLit("username", "password", "age"), 
            StrArrayLit("STRING", "STRING", "INTEGER"),
            StrArrayLit("Pet", "Residence", "Favourite_Decimal", "other_info"),
            StrArrayLit("STRING", "BLOB", "REAL", "STRING"),
            NULL);

    /* char* first_session_id  = HTTP_Auth_AddUserIfNotExists("writer,blogger", "Umpa", "Lumpa", 15, "Cats", "Somewhere in America!", 102.149981, "something", "not good", "really not good", NULL, NULL, "good"); */
    /* char* second_session_id = HTTP_Auth_AddUserIfNotExists("user", "Google", "Carmack", 29, "Dogs", "Somewhere in the Milky Way!", "harrow!", "something else", "not good", "really not good", NULL, NULL, "good"); */

    /* printf("%s\n", HTTP_CreateDateString(allocator.recycle_arena, StrLit("Mon"), 31, StrLit("Jun"), 2019, 7, 20, 27)); */

    char* dirs[] = {"something_elsdafj", "static/", };
    HTTP_SetSearchDirectories(dirs, ArrayCount(dirs));

    HTTP_HandleRoute(StrArrayLit("global", "second"), "GET", "/", false, root_page_handler);
    /* HTTP_HandleRoute(StrArrayLit("global", "second"), "GET", "/", true, second_page_handler); */
    HTTP_HandleRoute(StrArrayLit("global"), "POST", "/", false, root_page_handler);

    HTTP_HandleRoute(StrArrayLit("user"), "GET", "/login_page", false, login_form_handler);
    HTTP_HandleRoute(StrArrayLit("writer"), "GET", "/second_page", false, login_form_handler);

    HTTP_HandleRoute(StrArrayLit("global"), "GET", "/good", true, root_page_handler);
    HTTP_HandleRoute(StrArrayLit("global"), "POST", "/good", true, root_page_handler);
    HTTP_HandleRoute(StrArrayLit("global"), "UPDATE", "/good", true, root_page_handler);

    HTTP_HandleRoute(StrArrayLit("global"), "GET", "/som[0-9]*", true, second_page_handler);
    HTTP_HandleRoute(StrArrayLit("global"), "GET", "/som[0-9]*", false, root_page_handler);


    HTTP_DeleteRouteForAllMethod("/good", true);
    HTTP_Set404Page("static/error_page.html");

    /* HTTP_HandleRedirectRoute("GET", "/other main website", "/"); */

    printf("\n");

    printf("==========================\n");
    printf("Max session ID length: %d\n", SSL_MAX_SSL_SESSION_ID_LENGTH);
    printf("==========================\n");
    HTTP_RunServer("8000", "vendor/certs/cert.pem", "vendor/certs/key.pem");

#if 0


    ///////////////////

    HTTP_CreateDatabase("new.db");
    /* HTTP_RunSQLQuery("CREATE TABLE IF NOT EXISTS Info(fname TEXT, lname TEXT, pname TEXT, custom TEXT, name TEXT, surname TEXT)", false); */
    SessionMaxTimeout timeout = {
        .years = 10,
        .months = 20,
        .days = 40,
        .hours = 29,
        .minutes = 10,
        .seconds = 2000
    };

    HTTP_Auth_SessionEnable("SessionAuthTable",
            StrArrayLit("username", "password", "age"), 
            StrArrayLit("STRING", "STRING", "INTEGER"),
            StrArrayLit("Pet", "Residence", "Favourite_Decimal", "other_info"),
            StrArrayLit("STRING", "BLOB", "REAL", "STRING"),
            NULL);

    char* session_token = HTTP_Auth_AddUserIfNotExists("ing,else,not nice,really nice", "morning", "John", 5.42, "Cats", "Somewhere in America!", 102.149981, "something", "not good", "really not good", NULL, NULL, "good");

    cJSON* create_user_data = cJSON_CreateObject();
    cJSON_AddStringToObject(create_user_data, "Favourite_Decimal", "I love decimals!"); 
    cJSON_AddNumberToObject(create_user_data, "age", -1239);
    cJSON_AddStringToObject(create_user_data, "Pet", "avalon");
    cJSON_AddStringToObject(create_user_data, "Residence", "50");
    cJSON_AddNumberToObject(create_user_data, "some", 111);
    cJSON_AddStringToObject(create_user_data, "password", "BlowTorch");
    cJSON_AddStringToObject(create_user_data, "username", "Jonathan");
    cJSON_AddNumberToObject(create_user_data, "good stuff", 1020);
    cJSON_AddNumberToObject(create_user_data, "good stuff", 999);
    cJSON_AddStringToObject(create_user_data, "other_info", "nice!"); 

    HTTP_cJSON_AddPermissionArrayToObject(create_user_data, StrArrayLit("something", "else", "102.3", "something", " not good", "nice"));

    printf("create_user_data: `%s`\n", cJSON_Print(create_user_data));

    /* cJSON_AddArrayToObject(create_user_data, "permissions", "something,good,else, for morning"); */

    session_token = HTTP_Auth_cJSON_AddUserIfNotExists(allocator.recycle_arena, create_user_data);

    printf("Created Session Token: `%s`\n", session_token);
    printf("cJSON permissions at session token: `%s`\n", cJSON_Print(HTTP_Auth_cJSON_GetPermissionsAtSessionID(session_token)));

    StringArray user_permissions_array = HTTP_Auth_StringArray_GetPermissionsAtSessionID(allocator.recycle_arena, session_token);
    printf("Total count: %d\n", user_permissions_array.count);
    for (int i = 0; i < user_permissions_array.count; i++) {
        printf("count: %d, permission string: `%s`\n", i, user_permissions_array.array[i]);
    }


    char* check_permission = "something";
    printf("Is permission: `%s` allowed at session ID: %d\n", check_permission, HTTP_Auth_CheckPermissionAllowedAtSessionID(session_token, check_permission));
    printf("Finished...\n");

    printf("Successfully deleted permission `%s` at session ID: %d\n", check_permission, HTTP_Auth_DeletePermissionAtSessionID(session_token, check_permission));
    printf("Successfully added permission `Johnny Blow` at session ID: %d\n", HTTP_Auth_AddPermissionAtSessionID(session_token, "Johnny Blow"));
    printf("Permission `Johnny Blow` exists at session ID: %d\n", HTTP_Auth_CheckPermissionAllowedAtSessionID(session_token, "Johnny Blow"));
    printf("Set new permissions at session ID: %d\n", HTTP_Auth_SetPermissionsAtSessionID(session_token, StrArrayLit("this", "is", "a", "new", "permission", "string")));
    user_permissions_array = HTTP_Auth_StringArray_GetPermissionsAtSessionID(allocator.recycle_arena, session_token);
    printf("Total count: %d\n", user_permissions_array.count);
    for (int i = 0; i < user_permissions_array.count; i++) {
        printf("count: %d, permission string: `%s`\n", i, user_permissions_array.array[i]);
    }


    /* CookiesDict cookie_dict = { (char*[]){"SessionID"}, (char*[]){session_token}, 1}; */
    /* char* new_session_token = HTTP_Auth_CookiesDict_GenerateNewSessionTokenIfExpired(allocator.recycle_arena, cookie_dict); */
    /* printf("new_session_token: %s\n", new_session_token); */

    /* printf("Token exists: %d\n", HTTP_Auth_CheckSessionIDExists(session_token)); */
    /* printf("Token expired: %d\n", HTTP_Auth_CheckSessionIDExpired(session_token)); */
    /* printf("Token refresh expiry date: %d\n", HTTP_Auth_RefreshTokenExpiryDate(session_token)); */

    /* printf("User Data at token: `%s`\n", cJSON_Print(HTTP_Auth_GetUserDataAtSessionToken(session_token, true, true, true))); */
    /* printf("Login Data at token: `%s`\n", cJSON_Print(HTTP_Auth_GetLoginDataAtSessionToken(session_token, true, false))); */

    /* printf("Final: `%s`\n", cJSON_Print(HTTP_Auth_GetLoginAndUserDataAtSessionToken(session_token, true, true, true))); */




    /*     cJSON* new_user_obj = cJSON_CreateObject(); */
    /*     cJSON_AddStringToObject(new_user_obj, "username", "John"); */
    /*     cJSON_AddStringToObject(new_user_obj, "password", "Haack"); */
    /*     cJSON_AddNumberToObject(new_user_obj, "age", 101); */

    /*     cJSON* new_login_obj = cJSON_CreateObject(); */
    /*     cJSON_AddStringToObject(new_login_obj, "username", "good"); */
    /*     cJSON_AddStringToObject(new_login_obj, "password", "ajfla"); */
    /*     cJSON_AddStringToObject(new_login_obj, "age", "nota"); */
    /*     cJSON_AddNumberToObject(new_login_obj, "Pet", 20); */
    /*     cJSON_AddStringToObject(new_login_obj, "Residence", "sumbao..."); */
    /*     cJSON_AddStringToObject(new_login_obj, "Favourite_Decimal", "googa!!"); */
    /*     cJSON_AddNumberToObject(new_login_obj, "other_info", 104.135803291); */

    /*     printf("[BEFORE] Session ID For Login Data: `%s`\n", HTTP_Auth_GetSessionIDAtLoginDetails(allocator.recycle_arena, new_user_obj)); */
    /*     HTTP_Auth_SetLoginAndUserDataAtSessionToken(allocator.recycle_arena, session_token, new_login_obj); */
    /*     printf("[AFTER 1] Session ID For Login Data: `%s`\n", HTTP_Auth_GetSessionIDAtLoginDetails(allocator.recycle_arena, new_user_obj)); */
    /*     printf("[AFTER 2] Session ID For Login Data: `%s`\n", HTTP_Auth_GetSessionIDAtLoginDetails(allocator.recycle_arena, new_login_obj)); */
    /* HTTP_Auth_SetUserDataAtSessionToken(allocator.recycle_arena, session_token, new_login_obj); */
    /* printf("Changing login data: %s\n", HTTP_Auth_SetLoginDataAtSessionToken(allocator.recycle_arena, session_token, new_login_obj)); */

#endif


    return 0;
}
