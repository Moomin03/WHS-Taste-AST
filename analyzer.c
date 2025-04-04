#include <stdio.h>
#include <stdlib.h>
// gcc analyzer.c -o analyzer -ljansson
#include <jansson.h>
#include <string.h>

int count_ifs(json_t *node) {
    int count = 0;

    if (!node) return 0;

    if (json_is_object(node)) {
        json_t *nodetype = json_object_get(node, "_nodetype");
        if (nodetype && json_is_string(nodetype) && strcmp(json_string_value(nodetype), "If") == 0) {
            count++;
        }

        const char *key;
        json_t *value;
        json_object_foreach(node, key, value) {
            count += count_ifs(value);
        }
    } else if (json_is_array(node)) {
        size_t index;
        json_t *value;
        json_array_foreach(node, index, value) {
            count += count_ifs(value);
        }
    }

    return count;
}

void analyze_function(json_t *decl, int index) {
    const char *func_name = "";
    const char *return_type = "";
    int param_count = 0;
    int if_count = 0;

    // 함수 이름
    json_t *name_node = json_object_get(decl, "name");
    if (json_is_string(name_node)) {
        func_name = json_string_value(name_node);
    }

    // 리턴 타입
    json_t *type1 = json_object_get(decl, "type"); // FuncDecl part
    if (type1) {
        json_t *type2 = json_object_get(type1, "type"); // TypeDecl part
        if (type2) {
            json_t *type3 = json_object_get(type2, "type"); // IdentifierType part
            if (type3) {
                json_t *names = json_object_get(type3, "names");
                if (json_is_array(names) && json_array_size(names) > 0) {
                    json_t *first = json_array_get(names, 0);
                    if (json_is_string(first)) {
                        return_type = json_string_value(first);
                    }
                }
            }
        }
    }

    // 파라미터 수 및 정보
    json_t *args = json_object_get(type1, "args");
    json_t *params = NULL;
    if (args) {
        params = json_object_get(args, "params");
        if (json_is_array(params)) {
            param_count = json_array_size(params);
        }
    }

    // if 개수
    json_t *body = json_object_get(decl, "body");
    if_count = count_ifs(body);

    // 출력
    printf("-----------------------------\n");
    printf("함수 #%d:\n", index + 1);
    printf("  이름: %s\n", func_name);
    printf("  리턴 타입: %s\n", return_type);
    printf("  파라미터 수: %d\n", param_count);

    if (param_count > 0) {
        for (int i = 0; i < param_count; i++) {
            json_t *param = json_array_get(params, i);
            const char *param_name = "";
            const char *param_type = "";

            // 이름
            json_t *pname = json_object_get(param, "name");
            if (json_is_string(pname)) {
                param_name = json_string_value(pname);
            }

            // 타입
            json_t *ptype1 = json_object_get(param, "type");
            if (ptype1) {
                json_t *ptype2 = json_object_get(ptype1, "type");
                if (ptype2) {
                    json_t *names = json_object_get(ptype2, "names");
                    if (json_is_array(names) && json_array_size(names) > 0) {
                        json_t *first = json_array_get(names, 0);
                        if (json_is_string(first)) {
                            param_type = json_string_value(first);
                        }
                    }
                }
            }

            printf("    파라미터 #%d: 타입 = %s, 변수 = %s\n", i + 1, param_type, param_name);
        }
    }

    printf("  if 조건문 수: %d\n", if_count);
}

int main() {
    json_error_t error;
    json_t *root = json_load_file("ast.json", 0, &error);

    if (!root) {
        fprintf(stderr, "[Emergency] JSON 로딩 실패: %s\n", error.text);
        return 1;
    }

    int func_count = 0;

    // 전역 탐색
    if (json_is_object(root)) {
        json_t *exts = json_object_get(root, "ext");
        if (json_is_array(exts)) {
            size_t i;
            json_t *item;
            json_array_foreach(exts, i, item) {
                json_t *type_node = json_object_get(item, "_nodetype");
                if (type_node && json_is_string(type_node) &&
                    strcmp(json_string_value(type_node), "FuncDef") == 0) {
                    func_count++;
                }
            }
            printf("*****************************\n");
            printf("[Result] 총 함수 개수: %d\n", func_count);
            printf("*****************************\n\n");

            int count = 0;
            json_array_foreach(exts, i, item) {
                json_t *type_node = json_object_get(item, "_nodetype");
                if (type_node && json_is_string(type_node) &&
                    strcmp(json_string_value(type_node), "FuncDef") == 0) {
                    json_t *decl = json_object_get(item, "decl");
                    json_t *body = json_object_get(item, "body");

                    // decl 안에는 이름, 타입, 파라미터 등이 들어 있음!!
                    if (decl && json_is_object(decl)) {
                        json_object_set_new(decl, "body", body ? json_incref(body) : NULL);
                        analyze_function(decl, count++);
                    }
                }
            }
        }
    }

    json_decref(root);
    return 0;
}
