<?hh // decl /* -*- php -*- */
/**
 * Copyright (c) 2014, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the "hack" directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 */

const int INTL_MAX_LOCALE_LEN = 80;
const string INTL_ICU_VERSION = '52.1';
const string INTL_ICU_DATA_VERSION = '52.1';

function datefmt_create($locale, $date_type, $time_type, $timezone_str = null, $calendar = null, $pattern = null);
function datefmt_format($args = null, $array = null);
function datefmt_format_object($object, $format = null, $locale = null);
function datefmt_get_calendar($mf);
function datefmt_get_calendar_object($mf);
function datefmt_get_datetype($mf);
function datefmt_get_error_code($nf);
function datefmt_get_error_message($coll);
function datefmt_get_locale($mf);
function datefmt_get_pattern($mf);
function datefmt_get_timetype($mf);
function datefmt_get_timezone($mf);
function datefmt_get_timezone_id($mf);
function datefmt_is_lenient($mf);
function datefmt_localtime($formatter, $string, &$position = null);
function datefmt_parse($formatter, $string, &$position = null);
function datefmt_set_calendar($mf, $calendar);
function datefmt_set_lenient($mf);
function datefmt_set_pattern($mf, $pattern);
function datefmt_set_timezone($mf, $timezone);
function datefmt_set_timezone_id($mf, $timezone);
function grapheme_extract($arg1, $arg2, $arg3 = null, $arg4 = null, &$arg5 = null);
function grapheme_stripos($haystack, $needle, $offset = null);
function grapheme_stristr($haystack, $needle, $before_needle = null);
function grapheme_strlen($string);
function grapheme_strpos($haystack, $needle, $offset = null);
function grapheme_strripos($haystack, $needle, $offset = null);
function grapheme_strrpos($haystack, $needle, $offset = null);
function grapheme_strstr($haystack, $needle, $before_needle = null);
function grapheme_substr($string, $start, $length = null);
function intlcal_add(IntlCalendar $calendar, $field, $amount);
function intlcal_after(IntlCalendar $calendar, IntlCalendar $otherCalendar);
function intlcal_before(IntlCalendar $calendar, IntlCalendar $otherCalendar);
function intlcal_clear(IntlCalendar $calendar, $field = null);
function intlcal_create_instance($timeZone = null, $locale = null);
function intlcal_equals(IntlCalendar $calendar, IntlCalendar $otherCalendar);
function intlcal_field_difference(IntlCalendar $calendar, $when, $field);
function intlcal_from_date_time($dateTime);
function intlcal_get(IntlCalendar $calendar, $field);
function intlcal_get_actual_maximum(IntlCalendar $calendar, $field);
function intlcal_get_actual_minimum(IntlCalendar $calendar, $field);
function intlcal_get_available_locales();
function intlcal_get_day_of_week_type(IntlCalendar $calendar, $dayOfWeek);
function intlcal_get_error_code(IntlCalendar $calendar);
function intlcal_get_error_message(IntlCalendar $calendar);
function intlcal_get_first_day_of_week(IntlCalendar $calendar);
function intlcal_get_greatest_minimum(IntlCalendar $calendar, $field);
function intlcal_get_keyword_values_for_locale($key, $locale, $commonlyUsed);
function intlcal_get_least_maximum(IntlCalendar $calendar, $field);
function intlcal_get_locale(IntlCalendar $calendar, $localeType);
function intlcal_get_maximum(IntlCalendar $calendar, $field);
function intlcal_get_minimal_days_in_first_week(IntlCalendar $calendar);
function intlcal_get_minimum(IntlCalendar $calendar, $field);
function intlcal_get_now();
function intlcal_get_repeated_wall_time_option(IntlCalendar $calendar);
function intlcal_get_skipped_wall_time_option(IntlCalendar $calendar);
function intlcal_get_time(IntlCalendar $calendar);
function intlcal_get_time_zone(IntlCalendar $calendar);
function intlcal_get_type(IntlCalendar $calendar);
function intlcal_get_weekend_transition(IntlCalendar $calendar, $dayOfWeek);
function intlcal_in_daylight_time(IntlCalendar $calendar);
function intlcal_is_equivalent_to(IntlCalendar $calendar, IntlCalendar $otherCalendar);
function intlcal_is_lenient(IntlCalendar $calendar);
function intlcal_is_set(IntlCalendar $calendar, $field);
function intlcal_is_weekend(IntlCalendar $calendar, $date = null);
function intlcal_roll(IntlCalendar $calendar, $field, $amountOrUpOrDown = null);
function intlcal_set(IntlCalendar $calendar, $fieldOrYear, $valueOrMonth, $dayOfMonth = null, $hour = null, $minute = null, $second = null);
function intlcal_set_first_day_of_week(IntlCalendar $calendar, $dayOfWeek);
function intlcal_set_lenient(IntlCalendar $calendar, $isLenient);
function intlcal_set_minimal_days_in_first_week(IntlCalendar $calendar, $numberOfDays);
function intlcal_set_repeated_wall_time_option(IntlCalendar $calendar, $wallTimeOption);
function intlcal_set_skipped_wall_time_option(IntlCalendar $calendar, $wallTimeOption);
function intlcal_set_time(IntlCalendar $calendar, $date);
function intlcal_set_time_zone(IntlCalendar $calendar, $timeZone);
function intlcal_to_date_time(IntlCalendar $calendar);
function intlgregcal_create_instance($timeZoneOrYear = null, $localeOrMonth = null, $dayOfMonth = null, $hour = null, $minute = null, $second = null);
function intlgregcal_get_gregorian_change(IntlGregorianCalendar $calendar);
function intlgregcal_is_leap_year(IntlGregorianCalendar $calendar, $year);
function intlgregcal_set_gregorian_change(IntlGregorianCalendar $calendar, $date);
function intltz_count_equivalent_ids($zoneId);
function intltz_create_default();
function intltz_create_enumeration($countryOrRawOffset = null);
function intltz_create_time_zone($zoneId);
function intltz_create_time_zone_id_enumeration($zoneType, $region = null, $rawOffset = null);
function intltz_from_date_time_zone(DateTimeZone $dateTimeZone);
function intltz_get_canonical_id($zoneId, &$isSystemID = null);
function intltz_get_display_name(IntlTimeZone $timeZone, $isDaylight = null, $style = null, $locale = null);
function intltz_get_dst_savings(IntlTimeZone $timeZone);
function intltz_get_equivalent_id($zoneId, $index);
function intltz_get_error_code(IntlTimeZone $timeZone);
function intltz_get_error_message(IntlTimeZone $timeZone);
function intltz_get_gmt();
function intltz_get_id(IntlTimeZone $timeZone);
function intltz_get_offset(IntlTimeZone $timeZone, $date, $local, &$rawOffset, &$dstOffset);
function intltz_get_raw_offset(IntlTimeZone $timeZone);
function intltz_get_region($zoneId);
function intltz_get_tz_data_version();
function intltz_get_unknown();
function intltz_has_same_rules(IntlTimeZone $timeZone, ?IntlTimeZone $otherTimeZone = null);
function intltz_to_date_time_zone(IntlTimeZone $timeZone);
function intltz_use_daylight_time(IntlTimeZone $timeZone);
function locale_accept_from_http($arg1);
function locale_canonicalize($arg1);
function locale_compose($arg1);
function locale_filter_matches($arg1, $arg2, $arg3);
function locale_get_all_variants($arg1);
function locale_get_default();
function locale_get_display_language($arg1, $arg2);
function locale_get_display_name($arg1, $arg2);
function locale_get_display_region($arg1, $arg2);
function locale_get_display_script($arg1, $arg2);
function locale_get_display_variant($arg1, $arg2);
function locale_get_keywords($arg1);
function locale_get_primary_language($arg1);
function locale_get_region($arg1);
function locale_get_script($arg1);
function locale_lookup($arg1, $arg2, $arg3, $arg4);
function locale_parse($arg1);
function locale_set_default($arg1);
function msgfmt_create($locale, $pattern);
function msgfmt_format($nf, $args);
function msgfmt_format_message($locale, $pattern, $args);
function msgfmt_get_error_code($nf);
function msgfmt_get_error_message($coll);
function msgfmt_get_locale($mf);
function msgfmt_get_pattern($mf);
function msgfmt_parse($nf, $source);
function msgfmt_parse_message($locale, $pattern, $source);
function msgfmt_set_pattern($mf, $pattern);
function normalizer_is_normalized($input, $form = null);
function normalizer_normalize($input, $form = null);
function numfmt_create($locale, $style, $pattern = null);
function numfmt_format($nf, $num, $type = null);
function numfmt_format_currency($nf, $num, $currency);
function numfmt_get_attribute($nf, $attr);
function numfmt_get_error_code($nf);
function numfmt_get_error_message($nf);
function numfmt_get_locale($nf, $type = null);
function numfmt_get_pattern($nf);
function numfmt_get_symbol($nf, $attr);
function numfmt_get_text_attribute($nf, $attr);
function numfmt_parse($formatter, $string, $type = null, &$position = null);
function numfmt_parse_currency($formatter, $string, &$currency, &$position = null);
function numfmt_set_attribute($nf, $attr, $value);
function numfmt_set_pattern($nf, $pattern);
function numfmt_set_symbol($nf, $attr, $symbol);
function numfmt_set_text_attribute($nf, $attr, $value);
function resourcebundle_count($bundle);
function resourcebundle_create($locale, $bundlename, $fallback = null);
function resourcebundle_get($bundle, $index, $fallback = null);
function resourcebundle_get_error_code($bundle);
function resourcebundle_get_error_message($bundle);
function resourcebundle_locales($bundlename);

/*
class Locale {
  const int ACTUAL_LOCALE = 0;
  const int VALID_LOCALE = 1;
  const string DEFAULT_LOCALE = '';
  const string LANG_TAG = 'language';
  const string EXTLANG_TAG = 'extlang';
  const string SCRIPT_TAG = 'script';
  const string REGION_TAG = 'region';
  const string VARIANT_TAG = 'variant';
  const string GRANDFATHERED_LANG_TAG = 'grandfathered';
  const string PRIVATE_TAG = 'private';
  public static function acceptFromHttp($arg1);
  public static function canonicalize($arg1);
  public static function composeLocale($arg1);
  public static function filterMatches($arg1, $arg2, $arg3);
  public static function getAllVariants($arg1);
  public static function getDefault();
  public static function getDisplayLanguage($arg1, $arg2);
  public static function getDisplayName($arg1, $arg2);
  public static function getDisplayRegion($arg1, $arg2);
  public static function getDisplayScript($arg1, $arg2);
  public static function getDisplayVariant($arg1, $arg2);
  public static function getKeywords($arg1);
  public static function getPrimaryLanguage($arg1);
  public static function getRegion($arg1);
  public static function getScript($arg1);
  public static function lookup($arg1, $arg2, $arg3, $arg4);
  public static function parseLocale($arg1);
  public static function setDefault($arg1);
}
*/

class MessageFormatter {
  public function __construct($locale, $pattern);
  public static function create($locale, $pattern);
  public function format($args);
  public static function formatMessage($locale, $pattern, $args);
  public function getErrorCode();
  public function getErrorMessage();
  public function getLocale();
  public function getPattern();
  public function parse($source);
  public static function parseMessage($locale, $pattern, $args);
  public function setPattern($pattern);
}

class IntlDateFormatter {
  const int FULL = 0;
  const int LONG = 1;
  const int MEDIUM = 2;
  const int SHORT = 3;
  const int NONE = -1;
  const int GREGORIAN = 1;
  const int TRADITIONAL = 0;
  public function __construct($locale, $datetype, $timetype, $timezone = null, $calendar = null, $pattern = null);
  public static function create($locale, $datetype, $timetype, $timezone = null, $calendar = null, $pattern = null);
  public function format($args = null, $array = null);
  public static function formatObject($object, $format = null, $locale = null);
  public function getCalendar();
  public function getCalendarObject();
  public function getDateType();
  public function getErrorCode();
  public function getErrorMessage();
  public function getLocale();
  public function getPattern();
  public function getTimeType();
  public function getTimeZone();
  public function getTimeZoneId();
  public function isLenient();
  public function localtime($string, &$position = null);
  public function parse($string, &$position = null);
  public function setCalendar($which);
  public function setLenient($lenient);
  public function setPattern($pattern);
  public function setTimeZone($zone);
  public function setTimeZoneId($zone);
}

class ResourceBundle<T> implements Traversable<T> {
  public function __construct($locale, $bundlename, $fallback = null);
  public function count();
  public static function create($locale, $bundlename, $fallback = null);
  public function get($index, $fallback = null);
  public function getErrorCode();
  public function getErrorMessage();
  public static function getLocales($bundlename);
}

class IntlTimeZone {
  const int DISPLAY_SHORT = 1;
  const int DISPLAY_LONG = 2;
  const int DISPLAY_SHORT_GENERIC = 3;
  const int DISPLAY_LONG_GENERIC = 4;
  const int DISPLAY_SHORT_GMT = 5;
  const int DISPLAY_LONG_GMT = 6;
  const int DISPLAY_SHORT_COMMONLY_USED = 7;
  const int DISPLAY_GENERIC_LOCATION = 8;
  const int TYPE_ANY = 0;
  const int TYPE_CANONICAL = 1;
  const int TYPE_CANONICAL_LOCATION = 2;
  private function __construct();
  public static function countEquivalentIDs($zoneId);
  public static function createDefault();
  public static function createEnumeration($countryOrRawOffset = null);
  public static function createTimeZone($zoneId);
  public static function createTimeZoneIDEnumeration($zoneType, $region = null, $rawOffset = null);
  public static function fromDateTimeZone($zoneId);
  public static function getCanonicalID($zoneId, &$isSystemID = null);
  public function getDSTSavings();
  public function getDisplayName($isDaylight = null, $style = null, $locale = null);
  public static function getEquivalentID($zoneId, $index);
  public function getErrorCode();
  public function getErrorMessage();
  public static function getGMT();
  public function getID();
  public function getOffset($date, $local, &$rawOffset, &$dstOffset);
  public function getRawOffset();
  public static function getRegion($zoneId);
  public static function getTZDataVersion();
  public static function getUnknown();
  public function hasSameRules(IntlTimeZone $otherTimeZone);
  public function toDateTimeZone();
  public function useDaylightTime();
}

class IntlCalendar {
  const int FIELD_ERA = 0;
  const int FIELD_YEAR = 1;
  const int FIELD_MONTH = 2;
  const int FIELD_WEEK_OF_YEAR = 3;
  const int FIELD_WEEK_OF_MONTH = 4;
  const int FIELD_DATE = 5;
  const int FIELD_DAY_OF_YEAR = 6;
  const int FIELD_DAY_OF_WEEK = 7;
  const int FIELD_DAY_OF_WEEK_IN_MONTH = 8;
  const int FIELD_AM_PM = 9;
  const int FIELD_HOUR = 10;
  const int FIELD_HOUR_OF_DAY = 11;
  const int FIELD_MINUTE = 12;
  const int FIELD_SECOND = 13;
  const int FIELD_MILLISECOND = 14;
  const int FIELD_ZONE_OFFSET = 15;
  const int FIELD_DST_OFFSET = 16;
  const int FIELD_YEAR_WOY = 17;
  const int FIELD_DOW_LOCAL = 18;
  const int FIELD_EXTENDED_YEAR = 19;
  const int FIELD_JULIAN_DAY = 20;
  const int FIELD_MILLISECONDS_IN_DAY = 21;
  const int FIELD_IS_LEAP_MONTH = 22;
  const int FIELD_FIELD_COUNT = 23;
  const int FIELD_DAY_OF_MONTH = 5;
  const int DOW_SUNDAY = 1;
  const int DOW_MONDAY = 2;
  const int DOW_TUESDAY = 3;
  const int DOW_WEDNESDAY = 4;
  const int DOW_THURSDAY = 5;
  const int DOW_FRIDAY = 6;
  const int DOW_SATURDAY = 7;
  const int DOW_TYPE_WEEKDAY = 0;
  const int DOW_TYPE_WEEKEND = 1;
  const int DOW_TYPE_WEEKEND_OFFSET = 2;
  const int DOW_TYPE_WEEKEND_CEASE = 3;
  const int WALLTIME_FIRST = 1;
  const int WALLTIME_LAST = 0;
  const int WALLTIME_NEXT_VALID = 2;
  private function __construct();
  public function add($field, $amount);
  public function after(IntlCalendar $calendar);
  public function before(IntlCalendar $calendar);
  public function clear($field = null);
  public static function createInstance($timeZone = null, $locale = null);
  public function equals(IntlCalendar $calendar);
  public function fieldDifference($when, $field);
  public static function fromDateTime($dateTime);
  public function get($field);
  public function getActualMaximum($field);
  public function getActualMinimum($field);
  public static function getAvailableLocales();
  public function getDayOfWeekType($dayOfWeek);
  public function getErrorCode();
  public function getErrorMessage();
  public function getFirstDayOfWeek();
  public function getGreatestMinimum($field);
  public static function getKeywordValuesForLocale($key, $locale, $commonlyUsed);
  public function getLeastMaximum($field);
  public function getLocale($localeType);
  public function getMaximum($field);
  public function getMinimalDaysInFirstWeek();
  public function getMinimum($field);
  public static function getNow();
  public function getRepeatedWallTimeOption();
  public function getSkippedWallTimeOption();
  public function getTime();
  public function getTimeZone();
  public function getType();
  public function getWeekendTransition($dayOfWeek);
  public function inDaylightTime();
  public function isEquivalentTo(IntlCalendar $calendar);
  public function isLenient();
  public function isSet($field);
  public function isWeekend($date = null);
  public function roll($field, $amountOrUpOrDown);
  public function set($fieldOrYear, $valueOrMonth, $dayOfMonth = null, $hour = null, $minute = null, $second = null);
  public function setFirstDayOfWeek($dayOfWeek);
  public function setLenient($isLenient);
  public function setMinimalDaysInFirstWeek($numberOfDays);
  public function setRepeatedWallTimeOption($wallTimeOption);
  public function setSkippedWallTimeOption($wallTimeOption);
  public function setTime($date);
  public function setTimeZone($timeZone);
  public function toDateTime();
}

class IntlGregorianCalendar extends IntlCalendar {
  public function getGregorianChange();
  public function isLeapYear($year);
  public function setGregorianChange($date);
}

class IntlException extends Exception {
}

class IntlIterator<T> implements Iterator<T> {
  public function current();
  public function key();
  public function next();
  public function rewind();
  public function valid();
}

class IntlBreakIterator<T> implements Traversable<T> {
  const int DONE = -1;
  const int WORD_NONE = 0;
  const int WORD_NONE_LIMIT = 100;
  const int WORD_NUMBER = 100;
  const int WORD_NUMBER_LIMIT = 200;
  const int WORD_LETTER = 200;
  const int WORD_LETTER_LIMIT = 300;
  const int WORD_KANA = 300;
  const int WORD_KANA_LIMIT = 400;
  const int WORD_IDEO = 400;
  const int WORD_IDEO_LIMIT = 500;
  const int LINE_SOFT = 0;
  const int LINE_SOFT_LIMIT = 100;
  const int LINE_HARD = 100;
  const int LINE_HARD_LIMIT = 200;
  const int SENTENCE_TERM = 0;
  const int SENTENCE_TERM_LIMIT = 100;
  const int SENTENCE_SEP = 100;
  const int SENTENCE_SEP_LIMIT = 200;
  private function __construct();
  public static function createCharacterInstance($locale = null);
  public static function createCodePointInstance();
  public static function createLineInstance($locale = null);
  public static function createSentenceInstance($locale = null);
  public static function createTitleInstance($locale = null);
  public static function createWordInstance($locale = null);
  public function current();
  public function first();
  public function following($offset);
  public function getErrorCode();
  public function getErrorMessage();
  public function getLocale($locale_type);
  public function getPartsIterator($key_type = null);
  public function getText();
  public function isBoundary($offset);
  public function last();
  public function next($offset = null);
  public function preceding($offset);
  public function previous();
  public function setText($text);
}

class IntlRuleBasedBreakIterator<T> extends IntlBreakIterator<T> {
  public function getBinaryRules();
  public function getRuleStatus();
  public function getRuleStatusVec();
  public function getRules();
}

class IntlCodePointBreakIterator<T> extends IntlBreakIterator<T> {
  public function getLastCodePoint();
}

class IntlPartsIterator<T> extends IntlIterator<T> {
  const int KEY_SEQUENTIAL = 0;
  const int KEY_LEFT = 1;
  const int KEY_RIGHT = 2;
  public function getBreakIterator();
}
