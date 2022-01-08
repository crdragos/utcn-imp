// This file is part of the IMP project.

#include "interp.h"
#include "program.h"

#include <iostream>



// -----------------------------------------------------------------------------
void Interp::Run()
{
  for (;;) {
    auto op = prog_.Read<Opcode>(pc_);
    switch (op) {
      case Opcode::PUSH_FUNC: {
        Push(prog_.Read<size_t>(pc_));
        continue;
      }
      case Opcode::PUSH_PROTO: {
        Push(prog_.Read<RuntimeFn>(pc_));
        continue;
      }
      case Opcode::PUSH_INT: {
        Push(prog_.Read<int64_t>(pc_));
        continue;
      }
      case Opcode::PEEK: {
        auto idx = prog_.Read<unsigned>(pc_);
        Push(*(stack_.rbegin() + idx));
        continue;
      }
      case Opcode::POP: {
        Pop();
        continue;
      }
      case Opcode::CALL: {
        auto callee = Pop();
        switch (callee.Kind) {
          case Value::Kind::PROTO: {
            (*callee.Val.Proto) (*this);
            continue;
          }
          case Value::Kind::ADDR: {
            Push(pc_);
            pc_ = callee.Val.Addr;
            continue;
          }
          case Value::Kind::INT: {
            throw RuntimeError("cannot call integer");
          }
        }
        continue;
      }
      case Opcode::ADD: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        Push(lhs + rhs);
        continue;
      }
      case Opcode::MINUS: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        Push(lhs - rhs);
        continue;
      }
      // add case for MUL, DIV, REM
      case Opcode::MUL: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        Push(lhs * rhs);
        continue;
      }
      case Opcode::DIV: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        Push(lhs / rhs);
        continue;
      }
      case Opcode::REM: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        Push(lhs % rhs);
        continue;
      }
      // add case for comp operations
      case Opcode::EQUALEQUAL: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        std::int64_t result = 0;
        if (lhs == rhs) {
          result = 1;
        }
        Push(result);
        continue;
      }
      case Opcode::G: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        std::int64_t result = 0;
        if (lhs > rhs) {
          result = 1;
        }
        Push(result);
        continue;
      }
      case Opcode::GE: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        std::int64_t result = 0;
        if (lhs >= rhs) {
          result = 1;
        }
        Push(result);
        continue;
      }
      case Opcode::L: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        std::int64_t result = 0;
        if (lhs < rhs) {
          result = 1;
        }
        Push(result);
        continue;
      }
      case Opcode::LE: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        std::int64_t result = 0;
        if (lhs <= rhs) {
          result = 1;
        }
        Push(result);
        continue;
      }
      case Opcode::NE: {
        auto rhs = PopInt();
        auto lhs = PopInt();
        std::int64_t result = 0;
        if (lhs != rhs) {
          result = 1;
        }
        Push(result);
        continue;
      }
      case Opcode::RET: {
        auto depth = prog_.Read<unsigned>(pc_);
        auto nargs = prog_.Read<unsigned>(pc_);
        auto v = Pop();
        stack_.resize(stack_.size() - depth);
        pc_ = PopAddr();
        stack_.resize(stack_.size() - nargs);
        Push(v);
        continue;
      }
      case Opcode::JUMP_FALSE: {
        auto cond = Pop();
        auto addr = prog_.Read<size_t>(pc_);
        if (!cond) {
          pc_ = addr;
        }
        continue;
      }
      case Opcode::JUMP: {
        pc_ = prog_.Read<size_t>(pc_);
        continue;
      }
      case Opcode::STOP: {
        return;
      }
    }
  }
}
