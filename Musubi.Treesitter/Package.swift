// swift-tools-version:5.3

import Foundation
import PackageDescription

var sources = ["src/parser.c"]
if FileManager.default.fileExists(atPath: "src/scanner.c") {
    sources.append("src/scanner.c")
}

let package = Package(
    name: "TreeSitterMusubi",
    products: [
        .library(name: "TreeSitterMusubi", targets: ["TreeSitterMusubi"]),
    ],
    dependencies: [
        .package(name: "SwiftTreeSitter", url: "https://github.com/tree-sitter/swift-tree-sitter", from: "0.9.0"),
    ],
    targets: [
        .target(
            name: "TreeSitterMusubi",
            dependencies: [],
            path: ".",
            sources: sources,
            resources: [
                .copy("queries")
            ],
            publicHeadersPath: "bindings/swift",
            cSettings: [.headerSearchPath("src")]
        ),
        .testTarget(
            name: "TreeSitterMusubiTests",
            dependencies: [
                "SwiftTreeSitter",
                "TreeSitterMusubi",
            ],
            path: "bindings/swift/TreeSitterMusubiTests"
        )
    ],
    cLanguageStandard: .c11
)
