;; Major mode for editing AKL, and for running AKL under Emacs
;; Copyright (C) 1986, 1987 Free Software Foundation, Inc.
;; This file was copied from the Prolog mode written by
;; Masanobu UMEDA (umerin@flab.flab.fujitsu.junet) and modified
;; by Johan Andersson, Peter Olin, Mats Carlsson, and Johan Bevemyr,
;; SICS, Sweden. Modifications for AKL by Bjorn Danielsson and Ralph
;; Clarke Haygood, SICS.

;; This file is part of GNU Emacs.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY.  No author or distributor
;; accepts responsibility to anyone for the consequences of using it
;; or for whether it serves any particular purpose or works at all,
;; unless he says so in writing.  Refer to the GNU Emacs General Public
;; License for full details.

;; Everyone is granted permission to copy, modify and redistribute
;; GNU Emacs, but only under the conditions described in the
;; GNU Emacs General Public License.   A copy of this license is
;; supposed to have been given to you along with GNU Emacs so you
;; can know your rights and responsibilities.  It should be in a
;; file named COPYING.  Among other things, the copyright notice
;; and this notice must be preserved on all copies.
;; 
;; Add the following lines (substituting for the filename if necessary)
;; to your ~/.emacs to make the modes available and to make GNU Emacs
;; use AKL mode automatically when editing files with a .akl extension:
;;
;; (setq load-path (cons "/usr/local/lib/agents0.9" load-path))
;; (autoload 'run-agents "akl"
;;        	  "Start an AGENTS subprocess." t)
;; (autoload 'akl-mode "akl"
;;        	  "Major mode for editing AKL programs." t)
;; (setq auto-mode-alist (nconc '(("\\.akl$" . akl-mode)) auto-mode-alist))

(require 'comint)

(defvar akl-mode-syntax-table nil)
(defvar akl-mode-abbrev-table nil)
(defvar akl-mode-map (make-sparse-keymap))

(defvar agents-system (or (getenv "AGENTSSYSTEM") "agents")
  "AGENTS system used by run-agents.")

(defvar temp-akl-file (make-temp-name "/tmp/aklp"))

;;;(defun build-agents-command (commstring)
;;;  (concat "zap_file('"
;;;	  temp-akl-file "', '"
;;;	  (or (buffer-file-name) "user") "', " commstring ").\n"))
(defun build-agents-command (commstring)
  (concat commstring "('" temp-akl-file "').\n"))

(defvar akl-indent-width 8)
(defvar akl-indent-guard -4)

(if akl-mode-syntax-table
    ()
  (let ((table (make-syntax-table)))
    (modify-syntax-entry ?\t "    " table)
    (modify-syntax-entry ?\n ">   " table)
    (modify-syntax-entry ?\f ">   " table)
    (modify-syntax-entry ?_ "w   " table)
    (modify-syntax-entry ?\\ ".   " table)
    (modify-syntax-entry ?/ ".   " table)
    (modify-syntax-entry ?* ".   " table)
    (modify-syntax-entry ?+ ".   " table)
    (modify-syntax-entry ?- ".   " table)
    (modify-syntax-entry ?= ".   " table)
    (modify-syntax-entry ?% "<   " table)
    (modify-syntax-entry ?< ".   " table)
    (modify-syntax-entry ?> ".   " table)
    (modify-syntax-entry ?\' "\"   " table)
    (setq akl-mode-syntax-table table)))

(define-abbrev-table 'akl-mode-abbrev-table ())

(defun akl-mode-variables ()
  (set-syntax-table akl-mode-syntax-table)
  (setq local-abbrev-table akl-mode-abbrev-table)
  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat "^%%\\|^$\\|" page-delimiter)) ;'%%..'
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)
  (make-local-variable 'paragraph-ignore-fill-prefix)
  (setq paragraph-ignore-fill-prefix t)
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'akl-indent-line)
  (make-local-variable 'comment-start)
  (setq comment-start "%")
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "%+ *")
  (make-local-variable 'comment-column)
  (setq comment-column 48)
  (make-local-variable 'comment-indent-hook)
  (setq comment-indent-hook 'akl-comment-indent))

(defun akl-mode-commands (map)
  (define-key map "\t" 'akl-indent-line)
  (define-key map "\C-c\C-c" 'akl-compile-definition)
  (define-key map "\C-cc" 'akl-compile-region)
  (define-key map "\C-cC" 'akl-compile-buffer))

(akl-mode-commands akl-mode-map)


;;; AKL MODE

(defun akl-mode ()
  "Major mode for editing AKL programs.

The following commands are available:
\\{akl-mode-map}
Blank lines and `%%...' separate paragraphs.  `%' starts a comment.

Entry to this mode calls the value of akl-mode-hook if that value is
non-nil."
  (interactive)
  (kill-all-local-variables)
  (use-local-map akl-mode-map)
  (setq major-mode 'akl-mode)
  (setq mode-name "AKL")
  (akl-mode-variables)
  (run-hooks 'akl-mode-hook))

(defun akl-indent-line (&optional whole-exp)
  "Indent line as AKL code; with argument, indent rest of expression
rigidly with line (NOT IMPLEMENTED YET)."
  (interactive "p")
  (let ((indent (akl-indent-level))
	(pos (- (point-max) (point))) beg)
    (beginning-of-line)
    (setq beg (point))
    (skip-chars-forward " \t")

    (if (zerop (- indent (current-column)))
	nil
      (delete-region beg (point))
      (indent-to indent))

    (if (< akl-indent-guard -1)
	(akl-indent-after-guard))

    (if (> (- (point-max) pos) (point))
	(goto-char (- (point-max) pos)))))

(defun akl-indent-after-guard ()
  "If at guard, align text after guard correctly."
  (let ((indentation (current-column)) beg col)
    (cond ((looking-at "->")
	   (forward-char 2))
	  ((looking-at "\\?\\?")
	   (forward-char 2))
	  ((looking-at "[|!?]")
	   (forward-char 1)))
    (setq beg (point))
    (skip-chars-forward " \t")
    (delete-region beg (point))
    (cond ((looking-at "%") (setq col comment-column))
;;;;	  ((looking-at "$") (setq col (current-column)))
	  ((> (current-column) indentation)
	   (setq col (- indentation akl-indent-guard)))
	  (t (setq col (current-column))))
    (indent-to col)))

(defun looking-at-akl-guard ()
  (or (looking-at "->")
      (looking-at "[|!?]")))

(defun akl-indent-level ()
  "Compute AKL indentation level."
  (save-excursion
    (beginning-of-line)
    (skip-chars-forward " \t")
    (cond
     ((looking-at "%%%") 0)		;Large comment starts
     ((looking-at "%[^%]") comment-column) ;Small comment starts
     ((bobp) 0)				;Beginning of buffer
     ((looking-at "\n")                 ; a new fresh line
      (akl-indent-for-new-clause))
     ((looking-at-akl-guard)
      (forward-line -1)
      (+ akl-indent-width
	 akl-indent-guard
	 (if (looking-at "[ \t]")
	     (akl-indent-for-new-clause)
	   0)))
     (t                                 ; indent existing clause
      (forward-line -1)
      (akl-indent-for-new-clause)))))


(defun akl-search-for-prev-goal ()
  "Search for most recent AKL symbol (in head or body)."
  (while (and (not (bobp)) (or (looking-at "%[^%]") (looking-at "\n")))
    (forward-line -1)
    (skip-chars-forward " \t")))

(defun akl-indent-for-new-clause ()
  "Find column for new goal."
  (akl-search-for-prev-goal)
  (skip-chars-forward " \t")
  (let ((prevcol (current-column))
	(guardprevp (looking-at-akl-guard)))
    (end-of-akl-clause)
    (forward-char -1)
    (cond ((bobp) 0)
	  ((looking-at "[.]") 0)
	  ((zerop prevcol) akl-indent-width)
	  ((looking-at "[\[{(;]")
	   (+ akl-indent-width (akl-column-of-um-lparen)))
	  ((looking-at "[>|?!]")
	   (max 1 (- (akl-column-of-prev-term) akl-indent-guard)))
	  ((looking-at "[,]")
	   (if guardprevp
	       (max 1 (- (akl-column-of-prev-term) akl-indent-guard))
	     (akl-column-of-prev-term)))
	  (t (akl-column-of-um-lparen)))))

(defun akl-column-of-prev-term ()
  (beginning-of-line)
  (skip-chars-forward " \t\[{(;")
  (current-column))

(defun akl-column-of-um-lparen ()
  (let ((pbal 0))
    (while (and (>= pbal 0)
		(or (> (current-column) 0)
		    (looking-at "[ \t]")))
      (cond ((looking-at "[\]})]")
	     (setq pbal (1+ pbal))
	     (forward-char -1))
	    ((looking-at "[\[{(]")
	     (setq pbal (1- pbal))
	     (if (>= pbal 0) (forward-char -1)))
	    ((looking-at "'")
	     (search-backward "'" nil t)
	     (forward-char -1))
	    ((looking-at "\"")
	     (search-backward "\"" nil t)
	     (forward-char -1))
	    (t (forward-char -1)))))
  (current-column))

(defun end-of-akl-clause ()
  "Go to end of clause in line."
  (beginning-of-line)
  (let* ((eolpos (save-excursion (end-of-line) (point))))
    (if (re-search-forward comment-start-skip eolpos 'move)
	(goto-char (match-beginning 0)))
    (skip-chars-backward " \t")))

(defun akl-comment-indent ()
  "Compute AKL comment indentation."
  (cond ((looking-at "%%%") 0)
	((looking-at "%%") (akl-indent-level))
	(t
	 (save-excursion
	       (skip-chars-backward " \t")
	       (max (1+ (current-column)) ;Insert one space at least
		    comment-column)))))


(defvar agents-mode-map nil)

(defun agents-mode ()
  "Major mode for interacting with an inferior AGENTS process.

The following commands are available:
\\{agents-mode-map}
You can send text to the inferior AGENTS process from AKL-Mode buffers
using \\[akl-compile-definition], \\[akl-compile-region], and
\\[akl-compile-buffer].

Entry to this mode first calls the value of comint-mode-hook if that
value is non-nil and then calls the value of agents-mode-hook if that
value is non-nil."
  (interactive)
  (cond ((not (eq major-mode 'agents-mode))
	 (kill-all-local-variables)
	 (comint-mode)
	 (setq comint-input-filter 'agents-input-filter)
	 (setq major-mode 'agents-mode)
	 (setq mode-name "AGENTS")
	 (setq mode-line-process '(": %s"))
	 (akl-mode-variables)
	 (if agents-mode-map
	     nil
	   (setq agents-mode-map (copy-alist comint-mode-map)))
	 (use-local-map agents-mode-map)
	 (setq comint-prompt-regexp "^| [ ?][- ] *") ;Set AGENTS prompt pattern
	 (run-hooks 'agents-mode-hook))))

(defun agents-input-filter (str)
  (cond ((string-match "\\`\\s *\\'" str) nil) ;whitespace
	((not (eq major-mode 'agents-mode)) t)
	((= (length str) 1) nil)	;one character
	((string-match "\\`[rf] *[0-9]*\\'" str) nil) ;r(edo) or f(ail)
	(t t)))

(defun run-agents ()
  "Run an inferior AGENTS process with input and output via the buffer *agents*."
  (interactive)
  (let ((buff (buffer-name)))
    (switch-to-buffer (make-comint "agents" agents-system))
    (agents-mode)))

(defun ensure-agents-process ()
  (make-comint "agents" agents-system))

;;------------------------------------------------------------
;;Compiling
;;------------------------------------------------------------

(defun akl-compile-region (start end)
  "Compiles the region."
   (interactive "r")
   (ensure-agents-process)
   (save-excursion
     (akl-write-region start end temp-akl-file))
   (process-send-string "agents" (build-agents-command "compile"))
   (switch-to-buffer-other-window "*agents*"))


(defun akl-compile-buffer ()
  "Compiles the entire buffer."
  (interactive)
  (akl-compile-region (point-min) (point-max)))


(defun akl-compile-definition ()
  "Compiles the definition around point."
  (interactive)
  (let ((boundaries (akl-agent-boundaries)))
    (akl-compile-region (car boundaries) (cdr boundaries))))


;; Must be improved. Cannot handle agents with clauses
;; separated by newlines.

(defun akl-agent-boundaries ()
  ;; Find "beginning" of agent
  (beginning-of-line)
  (while (and (not (looking-at "\n")) (not (bobp)))
    (forward-line -1)
    (skip-chars-forward " \t"))
  (let ((start (point)))
    ;; Find "end" of agent
    (forward-line 1)
    (skip-chars-forward " \t")
    (while (and (not (looking-at "\n")) (not (eobp)))
      (forward-line 1)
      (skip-chars-forward " \t"))
    (cons start (point))))

(defun akl-write-region (minpoint maxpoint aklfilename)
  (let ((tmpbuffer (generate-new-buffer "tmpbuffer"))
	(buffercont (buffer-substring minpoint maxpoint)))
    (set-buffer tmpbuffer)
    (insert buffercont "\n")
    (write-region (point-min) (point-max)
		  (concat aklfilename ".akl")
		  nil nil)
    (kill-buffer tmpbuffer)))

;; not in use -matsc
(defun akl-switch-to-buffer-other-window (buffer)
  (let ((currently-selected-window (selected-window)))
    (switch-to-buffer-other-window buffer)
    (select-window currently-selected-window)))


;; With this handy function this file can be compiled as
;; emacs -batch -l akl.el -f compile-akl-elisp
(defun compile-akl-elisp ()
  (byte-compile-file "akl.el"))
