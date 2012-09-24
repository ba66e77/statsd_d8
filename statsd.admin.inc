<?php

/**
 * Page callback for StatsD administrative settings.
 *
 */
function statsd_admin_settings() {

  $form['statsd_host'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Host'),
    '#size'          => 25,
    '#description'   => t('The hostname, or IP address of the StatsD daemon. To minimize latency issue, use an IP whenever possible.'),
    '#default_value' => variable_get('statsd_host', '127.0.0.1'),
  );
  $form['statsd_port'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Port'),
    '#size'          => 5,
    '#description'   => t('The port of the StatsD daemon'),
    '#default_value' => variable_get('statsd_port', '8125'),
  );
  $form['statsd_sample_rate'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Sample Rate'),
    '#size'          => 2,
    '#description'   => t('StatsD can send a subset of events to Graphite. Choose a lower sample rate if you want to reduce the number of events being sent. Sample rates are between 0 and 1 (e.g. 0.1 implies 10% of events will be logged)'),
    '#default_value' => variable_get('statsd_sample_rate', 1),
  );
  $form['statsd_user_events'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Send User Events'),
    '#description'   => t('Enabling this feature will capture a count of user events in the following categories: active sessions, successful logins, failed logins, password resets'),
    '#default_value' => variable_get('statsd_user_events', TRUE),
  );
  $form['statsd_watchdog'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Use Watchdog'),
    '#description'   => t('Enabling this feature will capture a count of the severity and type of errors passed through watchdog. Alternatively, it may be less spammy to use the built-in StatsD client to only record events your application cares about.'),
    '#default_value' => variable_get('statsd_watchdog', TRUE),
  );
  $form['statsd_watchdog_level'] = array(
    '#type'          => 'select',
    '#title'         => t('Log Level'),
    '#description'   => t('If watchdog logging is enabled, only send data to StatsD within or above the selected threshold'),
    '#options'       => watchdog_severity_levels(),
    '#default_value' => variable_get('statsd_watchdog_level', WATCHDOG_WARNING),
  );
  $form['statsd_prefix'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Prefix'),
    '#size'          => 15,
    '#description'   => t('Use a prefix if you need to separate similar events (such as on different web servers). This prefix is added for calls (if enabled), as well as any calls via the built-in StatsD client. Do not include the period at the end of the prefix (e.g. use "myprefix" instead of "myprefix."'),
    '#default_value' => variable_get('statsd_prefix', NULL),
  );
  $form['statsd_suffix'] = array(
    '#type'          => 'textfield',
    '#title'         => t('Suffix'),
    '#size'          => 15,
    '#description'   => t('Use a suffix if you need to separate similar events (such as on different web servers). This suffix is added for calls (if enabled), as well as any calls via the built-in StatsD client. Do not include the period at the beginning of the suffix (e.g. use "mysuffix" instead of "mysuffix."'),
    '#default_value' => variable_get('statsd_suffix', NULL),
  );

  return system_settings_form($form);

}


/**
 * Validation handler for the administrative settings.
 *
 */
function statsd_admin_settings_validate($form, &$form_state) {

  $form_state['values']['statsd_host']        = trim($form_state['values']['statsd_host']);
  $form_state['values']['statsd_port']        = trim($form_state['values']['statsd_port']);
  $form_state['values']['statsd_sample_rate'] = trim($form_state['values']['statsd_sample_rate']);
  $form_state['values']['statsd_prefix']      = trim(rtrim($form_state['values']['statsd_prefix'], '.') );
  $form_state['values']['statsd_suffix']      = trim(ltrim($form_state['values']['statsd_suffix'], '.') );

  $sample_rate = $form_state['values']['statsd_sample_rate'];

  if (!is_numeric($sample_rate) || $sample_rate <= 0 || $sample_rate > 1) {
    form_set_error('statsd_sample_rate', t('The sample rate must be a value between 0 and 1') );
  }

}

