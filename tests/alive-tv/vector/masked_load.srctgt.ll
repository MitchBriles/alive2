declare <8 x i32> @llvm.masked.load.v8i32.p0 (ptr, i32, <8 x i1>, <8 x i32>)

define <8 x i32> @src0(ptr %p, <8 x i32> %v) {
    %v2 = call <8 x i32> @llvm.masked.load.v8i12.p0(ptr %p, i32 16, <8 x i1> <i1 false, i1 false, i1 false, i1 false, i1 false, i1 false, i1 false, i1 false>, <8 x i32> %v)
    ret <8 x i32> %v
}

define <8 x i32> @tgt0(ptr %p, <8 x i32> %v) {
  ret <8 x i32> %v
}

define <8 x i32> @src1(ptr %p, <8 x i32> %v) {
    %v2 = call <8 x i32> @llvm.masked.load.v8i12.p0(ptr %p, i32 16, <8 x i1> <i1 true, i1 true, i1 true, i1 true, i1 true, i1 true, i1 true, i1 true>, <8 x i32> %v)
    ret <8 x i32> %v2
}

define <8 x i32> @tgt1(ptr %p, <8 x i32> %v) {
  %v2 = load <8 x i32>, ptr %p, align 16
  ret <8 x i32> %v2
}
