require("nvim-treesitter").install({ "cpp" })
vim.lsp.enable("clangd")

-- Disable semantic tokens when any LSP client attaches to a buffer
vim.api.nvim_create_autocmd("LspAttach", {
	callback = function(args)
		local client = vim.lsp.get_client_by_id(args.data.client_id)
		if client then
			client.server_capabilities.semanticTokensProvider = nil
		end
	end,
})

vim.api.nvim_create_autocmd("FileType", {
	pattern = { "cpp" },
	callback = function()
		vim.treesitter.start()
	end,
})

require("conform").setup({
	formatters_by_ft = {
		cpp = { "clang-format" },
	},
	format_on_save = true,
})
