declare void @llvm.masked.store.v8i32.p0 (<8 x i32>, ptr, i32, <8 x i1>)

define void @src0(ptr %p, <8 x i32> %v) {
    call void @llvm.masked.store.v8i12.p0(<8 x i32> %v, ptr %p, i32 16, <8 x i1> <i1 false, i1 false, i1 false, i1 false, i1 false, i1 false, i1 false, i1 false>)
    ret void
}

define void @tgt0(ptr %p, <8 x i32> %v) {
  ret void
}

define void @src1(ptr %p, <8 x i32> %v) {
    call void @llvm.masked.store.v8i12.p0(<8 x i32> %v, ptr %p, i32 16, <8 x i1> <i1 true, i1 true, i1 true, i1 true, i1 true, i1 true, i1 true, i1 true>)
    ret void
}

define void @tgt1(ptr %p, <8 x i32> %v) {
  store <8 x i32> %v, ptr %p, align 16
  ret void
}
