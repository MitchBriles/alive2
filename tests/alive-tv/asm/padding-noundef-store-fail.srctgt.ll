; TEST-ARGS: -tgt-is-asm
; SKIP-IDENTITY
; ERROR: Mismatch in memory

define void @src(ptr %p, i16 %a, i16 %b) {
  %r = call noundef { i16, i1 } @llvm.sadd.with.overflow.i16(i16 %a, i16 %b)
  store { i16, i1 } %r, ptr %p, align 2
  ret void
}

define void @tgt(ptr %p, i16 %a, i16 %b) {
  %r = call { i16, i1 } @llvm.sadd.with.overflow.i16(i16 %a, i16 %b)
  %v = extractvalue { i16, i1 } %r, 0
  store i16 %v, ptr %p, align 2
  %q = getelementptr i8, ptr %p, i64 2
  store i8 0, ptr %q, align 1
  ret void
}

declare { i16, i1 } @llvm.sadd.with.overflow.i16(i16, i16)
