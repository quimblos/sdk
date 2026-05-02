#pragma once
#include "types.h"

qb_suite(test_type, "type", {

    auto checker = qb::TypeChecker();
    
    qb_describe("built-in", {
    
        qb_test("void", {
            auto type = checker.get(B_TYPE_VOID);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VOID)
            qb_assert(type->flags.of_void.is_void == true)
            qb_assert(type->flags.of_void.is_null == false)
        })
    
        qb_test("null", {
            auto type = checker.get(B_TYPE_NULL);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VOID)
            qb_assert(type->flags.of_void.is_void == false)
            qb_assert(type->flags.of_void.is_null == true)
        })
        
        qb_test("bool", {
            auto type = checker.get(B_TYPE_BOOL);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::BOOL)
        })

        qb_test("u8", {
            auto type = checker.get(B_TYPE_U8);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::INT)
            qb_assert(type->flags.of_int.is_unsigned == true)
            qb_assert(type->flags.of_int.res == 1)
        })

        qb_test("i8", {
            auto type = checker.get(B_TYPE_I8);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::INT)
            qb_assert(type->flags.of_int.is_unsigned == false)
            qb_assert(type->flags.of_int.res == 1)
        })

        qb_test("u16", {
            auto type = checker.get(B_TYPE_U16);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::INT)
            qb_assert(type->flags.of_int.is_unsigned == true)
            qb_assert(type->flags.of_int.res == 2)
        })

        qb_test("i16", {
            auto type = checker.get(B_TYPE_I16);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::INT)
            qb_assert(type->flags.of_int.is_unsigned == false)
            qb_assert(type->flags.of_int.res == 2)
        })

        qb_test("u32", {
            auto type = checker.get(B_TYPE_U32);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::INT)
            qb_assert(type->flags.of_int.is_unsigned == true)
            qb_assert(type->flags.of_int.res == 4)
        })

        qb_test("i32", {
            auto type = checker.get(B_TYPE_I32);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::INT)
            qb_assert(type->flags.of_int.is_unsigned == false)
            qb_assert(type->flags.of_int.res == 4)
        })

        qb_test("f32", {
            auto type = checker.get(B_TYPE_F32);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::FLOAT)
        })

        qb_test("str", {
            auto type = checker.get(B_TYPE_STR);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::STRING)
        })

        qb_test("ref", {
            auto type = checker.get(B_TYPE_REF);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::REF)
        })
    })

    qb_describe("map", {
    
        qb_test("map{bool}", {
            auto tdx = checker.add_map(B_TYPE_BOOL);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == true)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_BOOL))
        })
    
        qb_test("map{u8}", {
            auto tdx = checker.add_map(B_TYPE_U8);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == true)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_U8))
        })
    
        qb_test("map{f32}", {
            auto tdx = checker.add_map(B_TYPE_F32);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == true)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_F32))
        })
    
        qb_test("map{str}", {
            auto tdx = checker.add_map(B_TYPE_STR);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == true)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_STR))
        })

        qb_test("map{ref}", {
            auto tdx = checker.add_map(B_TYPE_REF);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == true)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_REF))
        })
    })

    qb_describe("struct", {
    
        qb_test("struct{bool}", {
            auto tdx = checker.add_struct({
                B_TYPE_BOOL
            });
            auto type = checker.get(tdx);
        
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == false)
            qb_assert(type->schema.of_struct.n_fields == 1)
            qb_assert(type->schema.of_struct.fields[0] == checker.get(B_TYPE_BOOL))
        })
    
        qb_test("struct{bool,u8}", {
            auto tdx = checker.add_struct({
                B_TYPE_BOOL,
                B_TYPE_U8
            });
            auto type = checker.get(tdx);
        
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == false)
            qb_assert(type->schema.of_struct.n_fields == 2)
            qb_assert(type->schema.of_struct.fields[0] == checker.get(B_TYPE_BOOL))
            qb_assert(type->schema.of_struct.fields[1] == checker.get(B_TYPE_U8))
        })
    
        qb_test("struct{bool,u8,f32}", {
            auto tdx = checker.add_struct({
                B_TYPE_BOOL,
                B_TYPE_U8,
                B_TYPE_F32
            });
            auto type = checker.get(tdx);
        
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == false)
            qb_assert(type->schema.of_struct.n_fields == 3)
            qb_assert(type->schema.of_struct.fields[0] == checker.get(B_TYPE_BOOL))
            qb_assert(type->schema.of_struct.fields[1] == checker.get(B_TYPE_U8))
            qb_assert(type->schema.of_struct.fields[2] == checker.get(B_TYPE_F32))
        })
    
        qb_test("struct{bool,u8,f32,str}", {
            auto tdx = checker.add_struct({
                B_TYPE_BOOL,
                B_TYPE_U8,
                B_TYPE_F32,
                B_TYPE_STR
            });
            auto type = checker.get(tdx);
        
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == false)
            qb_assert(type->schema.of_struct.n_fields == 4)
            qb_assert(type->schema.of_struct.fields[0] == checker.get(B_TYPE_BOOL))
            qb_assert(type->schema.of_struct.fields[1] == checker.get(B_TYPE_U8))
            qb_assert(type->schema.of_struct.fields[2] == checker.get(B_TYPE_F32))
            qb_assert(type->schema.of_struct.fields[3] == checker.get(B_TYPE_STR))
        })
    
        qb_test("struct{bool,u8,f32,str,ref}", {
            auto tdx = checker.add_struct({
                B_TYPE_BOOL,
                B_TYPE_U8,
                B_TYPE_F32,
                B_TYPE_STR,
                B_TYPE_REF
            });
            auto type = checker.get(tdx);
        
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == false)
            qb_assert(type->schema.of_struct.n_fields == 5)
            qb_assert(type->schema.of_struct.fields[0] == checker.get(B_TYPE_BOOL))
            qb_assert(type->schema.of_struct.fields[1] == checker.get(B_TYPE_U8))
            qb_assert(type->schema.of_struct.fields[2] == checker.get(B_TYPE_F32))
            qb_assert(type->schema.of_struct.fields[3] == checker.get(B_TYPE_STR))
            qb_assert(type->schema.of_struct.fields[4] == checker.get(B_TYPE_REF))
        })
    
    })

    qb_describe("event", {
    
        qb_test("event{bool}", {
            auto tdx = checker.add_event(B_TYPE_BOOL);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::EVENT)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_BOOL))
        })
    
        qb_test("event{u8}", {
            auto tdx = checker.add_event(B_TYPE_U8);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::EVENT)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_U8))
        })
    
        qb_test("event{f32}", {
            auto tdx = checker.add_event(B_TYPE_F32);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::EVENT)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_F32))
        })
    
        qb_test("event{str}", {
            auto tdx = checker.add_event(B_TYPE_STR);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::EVENT)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_STR))
        })
    
        qb_test("event{ref}", {
            auto tdx = checker.add_event(B_TYPE_REF);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::EVENT)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_REF))
        })
    })

    qb_describe("vec", {
    
        qb_test("vec{bool}", {
            auto tdx = checker.add_vec(B_TYPE_BOOL);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VECTOR)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_BOOL))
        })
    
        qb_test("vec[u8]", {
            auto tdx = checker.add_vec(B_TYPE_U8);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VECTOR)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_U8))
        })
    
        qb_test("vec{f32}", {
            auto tdx = checker.add_vec(B_TYPE_F32);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VECTOR)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_F32))
        })
    
        qb_test("vec{str}", {
            auto tdx = checker.add_vec(B_TYPE_STR);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VECTOR)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_STR))
        })
    
        qb_test("vec{ref}", {
            auto tdx = checker.add_vec(B_TYPE_REF);
            auto type = checker.get(tdx);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VECTOR)
            qb_assert(type->schema.of_map.type == checker.get(B_TYPE_REF))
        })
    })

    qb_describe("complex map", {
    
        qb_test("map{map{bool}}", {
            auto tdx0 = checker.add_map(B_TYPE_BOOL);
            auto tdx1 = checker.add_map(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == true)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    
        qb_test("map{struct{bool,u8}}", {
            auto tdx0 = checker.add_struct({
                B_TYPE_BOOL,
                B_TYPE_U8,
            });
            auto tdx1 = checker.add_map(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == true)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    
        qb_test("map{event{u8}}", {
            auto tdx0 = checker.add_event(B_TYPE_BOOL);
            auto tdx1 = checker.add_map(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == true)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    
        qb_test("map{vec[u8]}", {
            auto tdx0 = checker.add_vec(B_TYPE_BOOL);
            auto tdx1 = checker.add_map(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == true)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    })

    qb_describe("complex struct", {
    
        qb_test("struct{map{bool}}", {
            auto tdx0 = checker.add_map(B_TYPE_BOOL);
            auto tdx1 = checker.add_struct({
                tdx0
            });
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == false)
            qb_assert(type->schema.of_struct.n_fields == 1)
            qb_assert(type->schema.of_struct.fields[0] == checker.get(tdx0))
        })
    
        qb_test("struct{struct{bool,u8}}", {
            auto tdx0 = checker.add_struct({
                B_TYPE_BOOL,
                B_TYPE_U8,
            });
            auto tdx1 = checker.add_struct({
                tdx0
            });
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == false)
            qb_assert(type->schema.of_struct.n_fields == 1)
            qb_assert(type->schema.of_struct.fields[0] == checker.get(tdx0))
        })
    
        qb_test("struct{event{u8}}", {
            auto tdx0 = checker.add_event(B_TYPE_BOOL);
            auto tdx1 = checker.add_struct({
                tdx0
            });
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == false)
            qb_assert(type->schema.of_struct.n_fields == 1)
            qb_assert(type->schema.of_struct.fields[0] == checker.get(tdx0))
        })
    
        qb_test("struct{vec[u8]}", {
            auto tdx0 = checker.add_vec(B_TYPE_BOOL);
            auto tdx1 = checker.add_struct({
                tdx0
            });
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::OBJ)
            qb_assert(type->flags.of_obj.is_map == false)
            qb_assert(type->schema.of_struct.n_fields == 1)
            qb_assert(type->schema.of_struct.fields[0] == checker.get(tdx0))
        })
    })

    qb_describe("complex event", {
    
        qb_test("event{map{bool}}", {
            auto tdx0 = checker.add_map(B_TYPE_BOOL);
            auto tdx1 = checker.add_event(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::EVENT)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    
        qb_test("event{struct{bool,u8}}", {
            auto tdx0 = checker.add_struct({
                B_TYPE_BOOL,
                B_TYPE_U8,
            });
            auto tdx1 = checker.add_event(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::EVENT)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    
        qb_test("event{event{u8}}", {
            auto tdx0 = checker.add_event(B_TYPE_BOOL);
            auto tdx1 = checker.add_event(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::EVENT)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    
        qb_test("event{vec[u8]}", {
            auto tdx0 = checker.add_vec(B_TYPE_BOOL);
            auto tdx1 = checker.add_event(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::EVENT)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    })

    qb_describe("complex vec", {
    
        qb_test("vec[map{bool}]", {
            auto tdx0 = checker.add_map(B_TYPE_BOOL);
            auto tdx1 = checker.add_vec(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VECTOR)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    
        qb_test("vec[struct{bool,u8}]", {
            auto tdx0 = checker.add_struct({
                B_TYPE_BOOL,
                B_TYPE_U8,
            });
            auto tdx1 = checker.add_vec(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VECTOR)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    
        qb_test("vec[event{u8}]", {
            auto tdx0 = checker.add_event(B_TYPE_BOOL);
            auto tdx1 = checker.add_vec(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VECTOR)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    
        qb_test("vec[vec[u8]]", {
            auto tdx0 = checker.add_vec(B_TYPE_BOOL);
            auto tdx1 = checker.add_vec(tdx0);
            auto type = checker.get(tdx1);
            
            std::cout << type->to_str() << std::endl;
            qb_assert(type->kind == qb::TypeKind::VECTOR)
            qb_assert(type->schema.of_map.type == checker.get(tdx0))
        })
    })
})