package tree_sitter_musubi_test

import (
	"testing"

	tree_sitter "github.com/tree-sitter/go-tree-sitter"
	tree_sitter_musubi "github.com/schlafhase/musubi/bindings/go"
)

func TestCanLoadGrammar(t *testing.T) {
	language := tree_sitter.NewLanguage(tree_sitter_musubi.Language())
	if language == nil {
		t.Errorf("Error loading Musubi grammar")
	}
}
