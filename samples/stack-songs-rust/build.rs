use std::env;
use std::fs::File;
use std::io::Write;
use std::path::PathBuf;

use cc;

fn main() {
    // Put the memory.x script somewhere the linker can find it.
    let out = &PathBuf::from(env::var_os("OUT_DIR").unwrap());
    File::create(out.join("memory.x"))
        .unwrap()
        .write_all(include_bytes!("memory.x"))
        .unwrap();
    println!("cargo:rustc-link-search={}", out.display());

    // Only rerun the build script when any of these files have changed.
    println!("cargo:rerun-if-changed=memory.x");
    println!("cargo:rerun-if-changed=link.x");
    // println!("cargo:rerun-if-changed=../../include/libk/libk_wrapper.h");
    println!("cargo:rerun-if-changed=../../include/libk/libk.c");

    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let bindings = bindgen::Builder::default()
        // Make the generated code #![no_std] compatible.
        .ctypes_prefix("cty")
        .use_core()
        // The input header we would like to generate
        // bindings for.
        .header("../../include/libk/libk.c")
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");

    // Compile libk.
    cc::Build::new()
        // For some reason, it will only compile correctly if the file ends with .c
        .file("../../include/libk/libk.c")
        .compile("libk.a")
}
