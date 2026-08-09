#!/usr/bin/env perl

use strict;
use warnings;
use File::Temp qw(tempfile);

my $clang_format = $ENV{CLANG_FORMAT} // 'clang-format';
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

    next if $content eq $original;

    open my $out, '>', $file or die "failed to write $file: $!\n";
    print {$out} $content;
    close $out;
}

sub format_block {
    my ($open, $code, $close, $formatter) = @_;

    my ($base_indent) = $open =~ /^([ \t]*)/;
    $base_indent //= '';

    my @lines = split /\n/, $code, -1;

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
            push @indented, $base_indent . $line;
        } else {
            push @indented, '';
        }
    }

    my $result = join("\n", @indented);
    $result .= "\n";

    return $open . $result . $close;
}
