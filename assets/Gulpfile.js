var gulp = require('gulp');
var sass = require('gulp-sass');
var jshint = require('gulp-jshint');
var scsslint = require('gulp-scss-lint');

gulp.task('sass', function () {
	gulp.src('./scss/*.scss')
		.pipe(sass())
		.pipe(gulp.dest('./css'));
});

gulp.task('lintscss', function() {
	gulp.src('./scss/*.scss')
		.pipe(scsslint())
		.pipe(scsslint.failReporter());
});

gulp.task('lintjs', function () {
	return gulp.src('./js/*.js')
		.pipe(jshint())
		.pipe(jshint.reporter('default', {verbose: true}));
});

gulp.task('default', ['lintjs', 'sass', 'lintscss']);
