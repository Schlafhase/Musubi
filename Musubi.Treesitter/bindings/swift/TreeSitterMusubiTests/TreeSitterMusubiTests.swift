import XCTest
import SwiftTreeSitter
import TreeSitterMusubi

final class TreeSitterMusubiTests: XCTestCase {
    func testCanLoadGrammar() throws {
        let parser = Parser()
        let language = Language(language: tree_sitter_musubi())
        XCTAssertNoThrow(try parser.setLanguage(language),
                         "Error loading Musubi grammar")
    }
}
