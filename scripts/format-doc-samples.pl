#!/usr/bin/env perl

use strict;
use warnings;
use File::Temp qw(tempfile);
use Text::Wrap qw(wrap);

my $clang_format = $ENV{CLANG_FORMAT} // 'clang-format';
my $column_limit = $ENV{CLANG_FORMAT_COLUMN_LIMIT} // 120;
my @files = @ARGV;

if (!@files) {
    die "usage: format-doc-samples.pl <file> [<file> ...]\n";
}

for my $file (@files) {
    open my $in, '<', $file or die "failed to open $file: $!\n";
    local $/;
    my $content = <$in>;
    close $in;

    my $original = $content;

    $content =~ s{(^[ \t]*~~~~~\{\.cpp\}[ \t]*\n)(.*?)(^[ \t]*~~~~~[ \t]*$)}
               { format_block($1, $2, $3, $clang_format) }gmse;
    $content =~ s{(^[ \t]*/\*\*.*?^[ \t]*\*/[ \t]*$)}
                  { format_doxygen_block($1, $column_limit) }gmse;

    next if $content eq $original;

    open my $out, '>', $file or die "failed to write $file: $!\n";
    print {$out} $content;
    close $out;
}

sub format_doxygen_block {
    my ($block, $limit) = @_;
    my @lines = split /\n/, $block, -1;
    my @formatted;
    my @paragraph;
    my $in_fence = 0;

    my $flush_paragraph = sub {
        return if !@paragraph;

        my ($indent) = $paragraph[0] =~ /^([ \t]*)\*/;
        $indent //= '';
        my $prefix = $indent . '* ';
        my $text = join ' ', map {
            my (undef, $content) = /^([ \t]*)\*\s?(.*)$/;
            $content //= $_;
            $content =~ s/[ \t]+\z//;
            $content;
        } @paragraph;
        local $Text::Wrap::columns = $limit - length($prefix);
        my $wrapped = wrap('', '', $text);
        push @formatted, map { $prefix . $_ } split /\n/, $wrapped;
        @paragraph = ();
    };

    for my $line (@lines) {
        if ($line =~ /^[ \t]*\*\/[ \t]*$/) {
            $flush_paragraph->();
            push @formatted, $line;
        } elsif ($line =~ /~~~~~/) {
            $flush_paragraph->();
            $in_fence = !$in_fence;
            push @formatted, $line;
        } elsif ($in_fence || $line !~ /^[ \t]*\*\s?\S/) {
            $flush_paragraph->();
            push @formatted, $line;
        } elsif ($line =~ /^[ \t]*\*\s?(?:\\(?:param|tparam|return|throws|note|warning|anchor)\b|@)/) {
            $flush_paragraph->();
            push @formatted, $line;
        } else {
            push @paragraph, $line;
        }
    }
    $flush_paragraph->();

    return join("\n", @formatted);
}

sub format_block {
    my ($open, $code, $close, $formatter) = @_;

    my ($base_indent) = $open =~ /^([ \t]*)/;
    $base_indent //= '';

    my @lines = split /\n/, $code, -1;

    my $has_comment_prefix = 0;
    for my $line (@lines) {
        next if $line !~ /\S/;
        $has_comment_prefix = 1 if $line =~ /^[ \t]*\*[ \t]?/;
        last;
    }

    my $min_indent;
    for my $line (@lines) {
        next if $line !~ /\S/;
        my ($lead) = $line =~ /^([ \t]*)/;
        my $len = length($lead);
        if (!defined $min_indent || $len < $min_indent) {
            $min_indent = $len;
        }
    }
    $min_indent //= 0;

    for my $line (@lines) {
        if ($line =~ /\S/) {
            $line = substr($line, $min_indent);
            $line =~ s/^\*[ \t]?// if $has_comment_prefix;
        }
    }

    my $dedented = join("\n", @lines);
    $dedented .= "\n" if $dedented !~ /\n\z/;

    my ($tmpfh, $tmpname) = tempfile();
    print {$tmpfh} $dedented;
    close $tmpfh;

    my $formatted = qx{$formatter "$tmpname"};
    my $status = $?;
    unlink $tmpname;
    if ($status != 0) {
        die "clang-format failed for doc sample block\n";
    }

    my @formatted_lines = split /\n/, $formatted, -1;
    if (@formatted_lines && $formatted_lines[-1] eq '') {
        pop @formatted_lines;
    }

    my @indented;
    for my $line (@formatted_lines) {
        if ($line =~ /\S/) {
            push @indented, $base_indent . ($has_comment_prefix ? '* ' : '') . $line;
        } else {
            push @indented, $has_comment_prefix ? $base_indent . '*' : '';
        }
    }

    my $result = join("\n", @indented);
    $result .= "\n";

    return $open . $result . $close;
}
