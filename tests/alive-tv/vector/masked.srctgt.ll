declare void @llvm.masked.store.v8i8.p0 (<8 x i8>, ptr, i32, <8 x i1>)
declare <8 x i8> @llvm.masked.load.v8i8.p0 (ptr, i32, <8 x i1>, <8 x i8>)
declare i8 @llvm.vector.reduce.add.v8i8(<8 x i8> %a)
declare void @llvm.assume(i1)

define i8 @src0(<8 x i1> %mask) {
  %xt = zext <8 x i1> %mask to <8 x i8>
  %cnt = call i8 @llvm.vector.reduce.add.v8i8(<8 x i8> %xt)
  %cmp = icmp eq i8 %cnt, 2
  call void @llvm.assume(i1 %cmp)
  %p = alloca <8 x i8>
  store <8 x i8> <i8 1, i8 1, i8 1, i8 1, i8 1, i8 1, i8 1, i8 1>, ptr %p, align 8


  store <8 x i8> <i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2>, ptr %p, align 8

; this should optimize to "return 10"
;  call void @llvm.masked.store.v8i8.p0(<8 x i8> <i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2, i8 2>, ptr %p, i32 8, <8 x i1> %mask)


  %v = load <8 x i8>, ptr %p
  %cnt2 = call i8 @llvm.vector.reduce.add.v8i8(<8 x i8> %v)
  ret i8 %cnt2
}

define i8 @tgt0(<8 x i1> %mask) {
  ret i8 16
}

