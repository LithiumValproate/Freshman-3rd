fn main() {
    cxx_build::bridge("room.rs")
        .include(".")
        .include("../struct")
        .compile("im_cxx");

    println!("cargo:rerun-if-changed=room.rs");
    println!("cargo:rerun-if-changed=message.h");
    println!("cargo:rerun-if-changed=../struct/student.h");
}
